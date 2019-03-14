/** @file
 *  
 *  @brief Multithreaded demo of @c chops::shared_buffer and @c chops::wait_queue. 
 * 
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2019 by Thurman Gillespy
 *  3/14/19
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
#include <functional> // std::for_each, std::mem_fn
#include <atomic> // std::atomic, std::memory_order_relaxed

#include "queue/wait_queue.hpp"
#include "utility/shared_buffer.hpp"


/** Project Overview
 * 
 * This demo program shows how to use @c chops::wait_queue and @c chops::shared_buffer
 * in a multithreaded environment.
 * 
 * The program can have from 1...n Device threads that each put 20 random numbers into
 * device_q, a @c chops::wait_queue. Over 100 threads can be run sucessfully (default 
 * is 100). Each Device thread generates random numbers in its' own 'centile': 
 * thread 0: 0..99; thread 1: 100..199; thread 2: 200-299, etc.
 * 
 * The device_q numbers are read by 1 or more (default is 10) Data threads. The numbers
 * are sorted by centile. When 5 numbers in the same centile are collected, a string is
 * created that is placed into data_q, another wait_queue of type @c
 * chops::wait_queue<chops::const_shared_buffer>>, ie, the string is copied into a new
 * @c chops::const_shared_buffer in the data_q. The first number in the string is the
 * 'index' that correspons to that centile, and the remaining numbers are kept in the
 * original order they were produced.
 * 
 *    0 87 17 65 5 32
 *    8 870 813 808 827 874
 * 
 * The data_q is read by the DB (database) thread. The string is extracted from the
 * data_q and appened to the proper centile string. 
 * 
 * When the DB thread is finished, a 'Data Report' is printed. Each row contains the 
 * random nubmers created by a particular thread, in chronlogic order.
 * 
 * Data Report
 * [0]      30  71   2  99  60  74  11  70   4  41  83  90  14  72  68  68  88  51  28  78
 * [1]     103 110 186 136 152 187 165 169 108 157 116 138 136 132 153 144 159 144 120 110 
 * [2]     273 215 249 265 218 252 286 239 273 269 242 263 245 249 215 266 214 245 258 214 
 * [3]     387 303 312 350 323 331 343 374 363 371 390 335 367 317 397 341 394 399 341 366 
 * [4]     447 415 448 464 455 489 462 446 440 452 417 426 417 446 427 433 484 431 445 468 
 * 
 * Mutex locks and conditional variable signaling is handled within the @c chops::wait_queue
 * class. 
 * 
 */

// generate 20 random numbers in a centile based on the 'start number' passed to
// the @c Device::processData method.
// Place numbers into a shared @c chops::wait_queue
class Device {
private:
    const int INTERVAL = 20; // usec: how often to generate a random #
    const int NUM_LIMIT = 20; // how many numbers to generate
    chops::wait_queue<int>& device_q;
    std::atomic<int>& num_device_threads;
    bool finished = false;

public:
    Device(chops::wait_queue<int>& wait, std::atomic<int>& ndt) : 
            device_q(wait), num_device_threads(ndt) {};
    
    // generate a random number range [start_num * 100 ... start_num * 100 + 1)
    // every interval usec
    void generateData(int start_num) {
        //std::cerr << "thread: [" << start_num << "] Generate Data\n";
        std::srand(time(0));
        int num_count = 0;
        while (!finished) {
            // create random number every <INTERVAL> usec
            std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL));
            int val = (std::rand() % 100) + (start_num * 100);
            // put it in the wait_queue
            device_q.push(val);
            // we only want NUM_LIMIT random numbers
            if (num_count++ > NUM_LIMIT) {
                finished = true;
            } 
        }
        num_device_threads.fetch_sub(1);
    }
};

// Read numbers in device_q , sort by centile into @c std::vector<vector<int>>.
// When 5 numbers accumulated in a centile, create string to place
// into data_q, then empty that vector
class Data {
private:
    chops::wait_queue<int>& device_q;
    chops::wait_queue<chops::const_shared_buffer>& data_q;
    std::atomic<int>& num_device_threads;
    std::atomic<int>& num_data_threads;
    unsigned int num_sources = 0;
    std::vector<std::vector<int> > store;


public:
    Data(chops::wait_queue<int>& devq, chops::wait_queue<chops::const_shared_buffer>& datq,
        std::atomic<int>& ndevt, std::atomic<int>& ndatt, int num ) : 
        device_q(devq), data_q(datq), num_device_threads(ndevt), num_data_threads(ndatt),
        num_sources(num) {
        // initialize vector
        for (int i = 0; i < static_cast<int>(num_sources); i++) {
            std::vector<int> v;
            store.push_back(v);
        }
    };
    
    // read data from device_q (wait_queue), periodically send to data_q
    void processData() {
        //std::cerr << "thread: Process Data\n";

        int count = 1;
        // read from the device_q while Device threads active
        while (num_device_threads.load() > 0 ) {
            readData(count);
        }

        // finish up - read until device_q is empty
        while (!device_q.empty()) {
            readData(count);
        }

        finish();
    }

private:
    // read number from device_q, if any present, place into store vector
    // by index number. When 5 numbers are in a centile vector, call
    // formatData()
    void readData(int& count) {
        //std::cerr << ".";
        // short delay to slow the '.' down in stdout
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        
        if (device_q.empty()) {
            return;
        }
        //std::cerr << "|";
        std::optional<int> result = device_q.try_pop().value();
        //std::cerr << "@1";
        if (!result.has_value()) {
            return;
        }
        int val = result.value();
        //std::cerr << "|\n";

        int index =  val / 100;
        // insert into proper vector
        store.at(index).push_back(val);
        // if 5 elements, create string to send to DB
        if (store.at(index).size() >= 5) {
            formatData(index);
        }
    }

    // create a string to send to the DB thread via data_q
    void formatData(int index) {
        // first number in s is the index
        std::string s = std::to_string(index) + " ";
        // convert each int into a string, and append to s
        // add spaces for formatting
        // first number is the 'index' or centile
        // example: 8 870 813 808 827 874
        auto f = [&](int num) {
            s += (index == 0 ? " " : "");
            s += (num / 10 == 0 ? " " : "");
            s += (index < 10 ? " " : "");
            s += std::to_string(num) + " ";
        };
        std::for_each(store.at(index).begin(), store.at(index).end(), f);
        // clear that vector
        store.at(index).clear();
        // put the sting into data_q, using API for @d std::const_shared_queue
        // NOTE: data_q is of type chops::wait_queue<chops::const_shared_buffer>>
        data_q.emplace_push(s.c_str(), s.size() + 1);
    }

    // decrement @c num_data_threads
    void finish() {
        num_data_threads.fetch_sub(1);
    }
};

// (very) simple database that reads string from data_q and places into proper centile
// generate 'Data Report' on exit
class DB {
private:
    chops::wait_queue<chops::const_shared_buffer>& data_q;
    std::atomic<int>& num_data_threads;
    unsigned int num_sources = 0;
    std::vector<std::string> db;

public:
    DB(chops::wait_queue<chops::const_shared_buffer>& datq, std::atomic<int>& ndt, 
        int num) : data_q(datq), num_data_threads(ndt), num_sources(num) {
        // initialize vector<vector<string>>
        for (int i = 0; i < num; i++) {
            std::string str = "[" + std::to_string(i) + "]\t";
            db.push_back(str);
        }
    };
    
    void insert() {
        //std::cerr << "thread: Database\n";
        // read data while data_threads active
        while (num_data_threads.load() > 0) {
            // loop back if empty
            if (data_q.empty()) { 
                continue;
            }
            processData();
        }
        // after all data_threads closed, read any data left in data_q
        while (!data_q.empty()) {
            processData();
        }

        // database final report
        std::cout << "\nData Report" << std::endl;
        std::for_each(db.begin(), db.end(), 
                [] (std::string s) { std::cout << s << std::endl; });
    }

private:
    // extract string from data_q, append to db vector
    void processData() {
        // get string from data_q
        //std::cerr << "<";
        std::optional<chops::const_shared_buffer> buf = data_q.try_pop();
        //std::cerr << "@2";
        if (!buf.has_value()) {
            return;
        }
        const std::string str = (char *)(buf.value()).data();
        //std::cerr << ">\n";
        // get the index
        int index = std::stoi(str.substr(0, str.find_first_of(" ")));
        // append to proper vector
        db.at(index) += str.substr(str.find_first_of(" ") + 1,
                                   std::string::npos);
    }
};

int main() {
    // number of Device threads
    const unsigned int NUM_SOURCES = 100; // must be > 0
    // number of Data threads
    // BUG: keep num_data << num_sources, or you will suffer
    const int NUM_DATA = 10;

    chops::wait_queue<int> device_q;
    chops::wait_queue<chops::const_shared_buffer> data_q;
    std::atomic<int> num_device_threads(NUM_SOURCES);
    std::atomic<int> num_data_threads(NUM_DATA);
    
    std::vector<std::thread> threadListDevice;
    std::vector<std::thread> threadListData;
    
    // initialize class instances to run as separate threads
    Device dev(device_q, num_device_threads);
    Data data(device_q, data_q, num_device_threads, num_data_threads, NUM_SOURCES);
    DB db(data_q, num_data_threads, NUM_SOURCES);

    std::cout << "Processing data...\n";
    std::cout.flush();

    // create threads
    for (int i = 0; i < (int)NUM_SOURCES; i++) {
        threadListDevice.push_back(std::thread(&Device::generateData, &dev, i));
    }

    for (int i = 0; i < NUM_DATA; i++) {
        threadListData.push_back(std::thread(&Data::processData, &data));
    }

    std::thread dbThread(&DB::insert, &db);
    
    // join threads
    std::for_each(threadListDevice.begin(), threadListDevice.end(),
                    std::mem_fn(&std::thread::join));
    std::for_each(threadListData.begin(), threadListData.end(),
                    std::mem_fn(&std::thread::join));
    dbThread.join();

    return EXIT_SUCCESS;
}
