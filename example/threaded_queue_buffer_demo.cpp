/** @file
 *  
 *  @brief Multithreaded demo of @c chops::shared_buffer and @c chops::wait_queue. 
 * 
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2019 by Thurman Gillespy
 *  3/12/19
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  Sample make file:
 *  g++ -std=c++17 -I ~/Projects/utility-rack/include/ multithreadedDemo.cpp -lpthread 
 */

#include <iostream>
#include <vector>
#include <thread>
#include <functional> 
#include <mutex>
#include <condition_variable>
#include <chrono> // std:chrono:system_clock::now()
#include <atomic> // std::atomic, std::memory_order_relaxed

#include "queue/wait_queue.hpp"
#include "utility/shared_buffer.hpp"

using namespace std::chrono_literals;

/** Project Overview
 * 
 * This demo program shows how to use @c chops::wait_queue and @c chops::shared_buffer
 * in a multithreaded environment.
 * 
 * The program can have from 1...n Device threads that each puts 20 random numbers into
 * a wait_queue. Over 100 threads can be run sucessfully (default is 10). Each Device
 * thread generates random numbers in its' own 'centile': thread 1: 0..99; thread 2:
 * 100..199; thread 3: 200-299, etc.
 * 
 * The wait_queue numbers are read by 1 or more (default is 1) Data threads. The numbers
 * are sorted by centile. When 5 numbers in the same centile are collected, a string is
 * created that is placed into the shared_buffer. The first number in the string is the
 * 'index' that correspons to that centile, and the remaining numbers are kept in the
 * original order they were produced.
 * 
 *    0 87 17 65 5 32
 *    8 870 813 808 827 874
 * 
 * The shared_buffer is read by the DB (database) thread. Each incomming thread is 
 * appened to the proper centile string. When finished, a 'Data Report' is 
 * printed. Each row contains the random nubmers created by a particular thread.
 * 
 * Data Report
 * [0]      30  71   2  99  60  74  11  70   4  41  83  90  14  72  68  68  88  51  28  78
 * [1]     103 110 186 136 152 187 165 169 108 157 116 138 136 132 153 144 159 144 120 110 
 * [2]     273 215 249 265 218 252 286 239 273 269 242 263 245 249 215 266 214 245 258 214 
 * [3]     387 303 312 350 323 331 343 374 363 371 390 335 367 317 397 341 394 399 341 366 
 * [4]     447 415 448 464 455 489 462 446 440 452 417 426 417 446 427 433 484 431 445 468 
 * 
 * Mutex locks and conditional variable signaling is handled within the @c chops::wait_queue
 * class. @c std::thread, @c std::mutex and @c std::conditional_variable classes were used
 * for the @c shared_buffer.
 * 
 */

// generate 20 random numbers in a centile based on the 'start number' passed to
// the @c Device::processData method.
// Place numbers into a shared @c chops::wait_queue
class Device {
private:
    chops::wait_queue<int>& wq;
    const int INTERVAL = 200; // usec: how often to generate a random #
    const int NUM_LIMIT = 20; // how many numbers to generate
    bool finished = false;

public:
    Device(chops::wait_queue<int>& wait) : wq(wait) {};
    
    // generate a random number range [start_num * 100 ... start_num * 100 + 1)
    // every interval usec
    void generateData(int start_num) {
        std::cerr << "thread: Generate Data\n";
        std::srand(time(0));
        int num_count = 0;
        while (!finished) {
            // create random number every <INTERVAL> usec
            std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL));
            int val = (std::rand() % 100) + (start_num * 100);
            // put it in the wait_queue
            wq.push(val);
            // we only want NUM_LIMIT random numbers
            if (num_count++ > NUM_LIMIT) {
                finished = true;
            } 
        }
    }
};

// Read numbers in wait_queue, sort by centile into @c std::vector<vector<int>>.
// When 5 numbers accumulated in a centile, create string to place
// into shared_buffer, then empty that vector
class Data {
private:
    std::mutex& mutex_buffer;
    std::condition_variable& cv;
    chops::wait_queue<int>& wq;
    chops::mutable_shared_buffer& buf;
    bool& data_ready; // signal to DB thread
    //int& num_data_threads; // signal to DB thread
    std::atomic<int>& num_data_threads;
    unsigned int num_sources = 0;
    std::vector<std::vector<int> > store;


public:
    Data(std::mutex& m_buffer, std::condition_variable& conv,
        chops::wait_queue<int>& queue, chops::mutable_shared_buffer& buffer, 
        bool& d_ready, /*int& ndt*/std::atomic<int>& ndt, int num ) :
        mutex_buffer(m_buffer), cv(conv), wq(queue), buf(buffer),
        data_ready(d_ready), num_data_threads(ndt), num_sources(num) {
        // initialize vector
        for (int i = 0; i < static_cast<int>(num_sources); i++) {
            std::vector<int> v;
            store.push_back(v);
        }
    };
    
    // read data from wait_queue, periodically send to shared_buffer
    void processData() {
        std::cerr << "thread: Process Data\n";
        // make sure there is data in the wait_queue
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        int delay = (300/num_sources < 20 ? 20 : 300/num_sources);
        int count = 1;
        while (!wq.empty()) {
            std::cerr << ".";
            if (count++ % 90 == 0) {
                std::cerr << std::endl;
            }
            // short delay ensures there is always data in the queue
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            
            int val = wq.try_pop().value();
            int index = val/100;
            // insert into proper vector
            store.at(index).push_back(val);
            // if 5 elements, create string to send to DB
            if (store.at(index).size() >= 5) {
                // first number in s is the index
                std::string s = std::to_string(index) + " ";
                // convert each int into a string, and append to s
                // add spaces for formatting
                // first number is the 'index' or centile
                // example: 8 870 813 808 827 874
                auto f = [&] (int num) {
                    s += (index == 0 ? " " : "");
                    s += (num/10 == 0 ? " ":"");
                    s += std::to_string(num) + " ";
                };
                std::for_each(store.at(index).begin(), store.at(index).end(), f);
                // clear that vector
                store.at(index).clear();
                // add string to buffer
                std::lock_guard<std::mutex> guard(mutex_buffer);
                buf.append(s.c_str(), s.size() + 1);
                // signal that data in shared_buffer is readly for DB thread
                data_ready = true;
                cv.notify_one();
            }
        }
        std::cerr << std::endl;
        data_ready = false;
        --num_data_threads;
        //std::cerr << "\nexit Data thread: num_data_threads: " << num_data_threads << std::endl;
    }
};

// (very) simple database that reads random numbers placed in shared_buffer,
// and places into proper centile
// generate 'Data Report' on exit
class DB {
private:
    std::mutex& mutex_buffer;
    std::condition_variable& cv;
    chops::wait_queue<int>& wq;
    chops::mutable_shared_buffer& buf;
    bool& data_ready;
    std::atomic<int>& num_data_threads;
    unsigned int num_sources = 0;
    std::vector<std::string> db;

public:
    DB(std::mutex& m, std::condition_variable& c, chops::wait_queue<int>& queue, 
        chops::mutable_shared_buffer& b, bool& d_ready, std::atomic<int>& ndt, int num) :
        mutex_buffer(m), cv(c), wq(queue), buf(b), data_ready(d_ready),
        num_data_threads(ndt), num_sources(num) {
        // initialize vector<vector<string>>
        for (int i = 0; i < num; i++) {
            std::string str = "[" + std::to_string(i) + "]\t";
            db.push_back(str);
        }
    };

    // accessor used by condtion_variable
    bool isDataReady() { return data_ready; }
    
    void insert() {
        std::cerr << "thread: Database\n";
        while (num_data_threads > 0) {
            auto now = std::chrono::system_clock::now();  
            std::unique_lock<std::mutex> mlock(mutex_buffer);
            
            cv.wait_until(mlock, now + 200ms, 
                std::bind(&DB::isDataReady, this));
           
            if (buf.empty()) { // nothing to read, back to top of loop
                continue;
            }
            // get string from buffer
            std::string str = (char *)buf.data();
            // get the index
            int index = std::stoi(str.substr(0, str.find_first_of(" ")));
            // append to proper vector
            db.at(index) += str.substr(str.find_first_of(" ") + 1,
                                       std::string::npos);
            // clear the buffer, reset flag
            buf.clear();
            data_ready = false;
        }

        // database final report
        std::cout << "Data Report" << std::endl;
        std::for_each(db.begin(), db.end(), 
                [] (std::string s) { std::cout << s << std::endl; });
    }
};

int main() {
    // number of Device threads
    const unsigned int num_sources = 10; // must be > 0
    // number of Data threads
    const int num_data = 1; // BUG: can only be 1

    std::vector<std::thread> threadListDevice;
    std::vector<std::thread> threadListData;
    chops::wait_queue<int> wq;
    chops::mutable_shared_buffer buf;
    std::mutex mutex_buffer; // shared_buffer
    std::condition_variable cv;
    bool data_ready = false; // data ready for read in shared_buffer
    std::atomic<int> num_data_threads(num_data);
    
    // initialize class instances to run as separate threads
    Device dev(wq);
    Data data(mutex_buffer, cv, wq, buf, data_ready, num_data_threads, num_sources);
    DB db(mutex_buffer, cv, wq, buf, data_ready, num_data_threads, num_sources);

    for (int i = 0; i < (int)num_sources; i++) {
        threadListDevice.push_back(std::thread(&Device::generateData, &dev, i));
    }

    for (int i = 0; i < num_data; i++) {
        threadListData.push_back(std::thread(&Data::processData, &data));
    }

    std::thread dbThread(&DB::insert, &db);
    
    std::for_each(threadListDevice.begin(), threadListDevice.end(),
                    std::mem_fn(&std::thread::join));
    std::for_each(threadListData.begin(), threadListData.end(),
                    std::mem_fn(&std::thread::join));
    dbThread.join();

    return EXIT_SUCCESS;
}
