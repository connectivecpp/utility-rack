/** @file
 *  
 *  @brief Multithreaded demo of @c chops::shared_buffer and @c chops::wait_queue. 
 * 
 *  @ingroup example_module
 *
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2019 by Thurman Gillespy
 *  3/22/19
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  Sample make file:
 *  g++ -std=c++17 -I ~/Projects/utility-rack/include/ multithreaded_demo.cpp -lpthread 
 */

#include <iostream>
#include <cstdlib> // std::srand, rand; EXIT_SUCCESS
#include <vector>
#include <string>
#include <thread> // std::thread
#include <functional> // std::mem_fn
#include <algorithm> // std::for_each
#include <atomic> // std::atomic
#include <future> // std::promise, std::future
#include <ctime> // std::time

#include "queue/wait_queue.hpp"
#include "marshall/shared_buffer.hpp"


/** Project Overview
 * 
 * This demo program shows how to use @c chops::wait_queue and @c chops::shared_buffer
 * in a multithreaded environment. The program simulates multiple peripheral data
 * generators, perhaps sensors or network connections. THe data is handled by one or
 * more data processors, which sorts and formats the data, and periodically sends it to
 * a simulated database.
 * 
 * The program can have 1 or more DeviceDataGenerator threads that each put 20 random
 * numbers into device_q, a @c chops::wait_queue. Over 1000 threads can be run sucessfully
 * (default is 20). Each DeviceDataGenerator thread generates random numbers in its' own
 * 'centile': thread 0: <0..99>; thread 1: <100..199>; thread 2: <200-299>, etc.
 * 
 * The device_q numbers are read by 1 or more (default is 5) DataProcessor threads.
 * The numbers are sorted by centile. When 5 numbers in the same centile are collected,
 * a string with the numbers is created that is placed into data_q, another wait_queue 
 * of type @c chops::wait_queue<chops::const_shared_buffer>>, ie, the string is copied into
 * a new @c chops::const_shared_buffer in the data_q. The first number in the string is the
 * 'index' that correspons to that centile, and the remaining numbers are kept in
 * chronologic order.
 * 
 *    0 87 17 65 5 32s
 *    8 870 813 808 827 874
 * 
 * The data_q is read by the single Database thread. The string is extracted from the
 * data_q and appened to the proper centile string in the database. 
 * 
 * When the Database thread is finished, a 'Data Report' is printed. Each row contains the 
 * random nubmers created by a particular thread, in chronlogic order.
 * 
 * Data Report
 * [0]      30  71   2  99  60  74  11  70   4  41  83  90  14  72  68  68  88  51  28  78
 * [1]     103 110 186 136 152 187 165 169 108 157 116 138 136 132 153 144 159 144 120 110 
 * [2]     273 215 249 265 218 252 286 239 273 269 242 263 245 249 215 266 214 245 258 214 
 * [3]     387 303 312 350 323 331 343 374 363 371 390 335 367 317 397 341 394 399 341 366 
 * [4]     447 415 448 464 455 489 462 446 440 452 417 426 417 446 427 433 484 431 445 468 
 * 
 * Mutex locks and conditional variable signaling is handled within the 
 * @c chops::wait_queue class. 
 * 
 */

// @c wait_queue for moving data between threads
using device_q_t = chops::wait_queue<int>;
using data_q_t = chops::wait_queue<chops::const_shared_buffer>;

// utility function for casting @c wait_queue.data()
template <class C>
const char* cast_to_char_ptr (const C buf) {
    return static_cast<const char*> (static_cast<const void*> (buf.data()));
}

// generate 20 random numbers in a centile based on the 'start number' passed to
// the @c Device::processData method.
// Place numbers into a shared @c chops::wait_queue
class DeviceDataGenerator {
private:
    constexpr static int INTERVAL = 20; // usec: how often to generate a random #
    constexpr static int NUM_LIMIT = 20; // how many numbers to generate
    
    device_q_t& m_device_q;
    std::atomic<int>& m_num_device_threads;
    const int m_start_num;

public:
    constexpr DeviceDataGenerator(device_q_t& wait, std::atomic<int>& dev_threads,
        const int start_num) : m_device_q(wait), m_num_device_threads(dev_threads),
        m_start_num(start_num) {};
    
    // generate a random number, range [(0...99) + start_num * 100]
    // every INTERVAL usec
    void operator()() {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        int num_count = NUM_LIMIT;
        // create NUM_LIMIT random numbers
        while (num_count-- > 0) {
            // create random number every INTERVAL usec
            std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL));
            const int val = (std::rand() % 100) + (m_start_num * 100);
            // put it in the wait_queue
            m_device_q.push(val);
        }
        
        // cleanup: decrement atomic number of device threads
        m_num_device_threads.fetch_sub(1);
    }
};

// Read numbers in m_device_q, sort by centile into @c std::vector<vector<int>>.
// When 5 numbers accumulated in a centile, create string to place
// into data_q, then empty that vector.
class DataProcessor {
private:
    device_q_t& m_device_q;
    data_q_t& m_data_q;
    std::atomic<int>& m_num_device_threads;
    std::atomic<int>& m_num_data_threads;
    const int m_num_devices;

    std::vector<std::vector<int> > m_store;

public:
    DataProcessor(device_q_t& devq, data_q_t& datq,
        std::atomic<int>& device_threads, std::atomic<int>& data_threads, const int num_devices) : 
        m_device_q(devq), m_data_q(datq), m_num_device_threads(device_threads),
        m_num_data_threads(data_threads), m_num_devices(num_devices) {
        
        // initialize vector
        for (int i = 0; i < m_num_devices; ++i) {
            m_store.push_back(std::vector<int>());
        }
    };
    
    // read data from m_device_q (wait_queue), periodically send to m_data_q
    void operator()() {
        // read from the m_device_q while DeviceDataGenerator threads active
        while (m_num_device_threads.load() > 0 ) {
            readData();
        }

        // finish up - read until m_device_q is empty
        while (!m_device_q.empty()) {
            readData();
        }

        // make sure nothing left in m_store vector<int>
        cleanup();

        // exit: subtract 1 from atomic data processor thread count
        m_num_data_threads.fetch_sub(1);
    }

private:
    // read number from m_device_q, if any present, place into m_store vector
    // by index number. When 5 numbers are in a centile vector, call
    // formatData()
    void readData() {

        if (m_device_q.empty()) {
            return;
        }
       
        std::optional<int> result = m_device_q.try_pop();
        if (!result) {
            return;
        }

        const int val = result.value();
        const int index =  val / 100;

        // insert into proper vector
        m_store.at(index).push_back(val);
        // if 5 elements, create string to send to DB
        if (m_store.at(index).size() >= 5) {
            formatData(index);
        }
    }

    // create a string to send to the DB thread via m_data_q
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
        std::for_each(m_store.at(index).begin(), m_store.at(index).end(), f);
        // clear that vector
        m_store.at(index).clear();
        // put the sting into m_data_q, using API for @d std::const_shared_queue
        m_data_q.emplace_push(s.c_str(), s.size() + 1);
    }

    // process any numbers left in m_store vector<int>
    void cleanup() {
        for (int index = 0; index < static_cast<int> (m_store.size()); index++) {
            if (!m_store.at(index).empty()) {
                formatData(index);
            }
        }
    }
};

// (very) simple database that reads string from m_data_q and places into proper centile
// generate 'Data Report' on exit
class Database {
private:
    data_q_t& m_data_q;
    std::atomic<int>& m_num_data_threads;
    std::promise<std::string>& m_promise;
    const unsigned int m_num_devices;

    std::vector<std::string> db;

public:
    Database(data_q_t& datq, std::atomic<int>& data_threads,
    std::promise<std::string>& promise, int num_devices) : m_data_q(datq),  
        m_num_data_threads(data_threads), m_promise(promise), m_num_devices(num_devices) {
        // initialize vector<vector<string>>
        for (int i = 0; i < static_cast<int> (m_num_devices); i++) {
            std::string str = "[" + std::to_string(i) + "]\t";
            db.push_back(str);
        }
    };
    
    // read data from m_data_q
    void operator()() {
        // read data while data_threads active
        while (m_num_data_threads.load() > 0) {
            // loop back if empty
            if (m_data_q.empty()) { 
                continue;
            }
            processData();
        }
        
        // after all data_threads closed, read any data left in m_data_q
        while (!m_data_q.empty()) {
            processData();
        }

        // database final report
        createReport();
    }

private:
    // extract string from m_data_q, append to db vector
    void processData() {
        // get string from m_data_q
        std::optional<chops::const_shared_buffer> buffer = m_data_q.try_pop();
        if (!buffer) {
            return;
        }
        
        const std::string str = cast_to_char_ptr (buffer.value());
        // get the index
        const int index = std::stoi(str.substr(0, str.find_first_of(" ")));
        // append to proper vector
        db.at(index) += str.substr(str.find_first_of(" ") + 1,
                                   std::string::npos);
    }

    // create data report, place into promise object for printing
    void createReport() const {
        std::cout << "\nData Report" << std::endl;
        // create report string
        std::string s_out;
        std::for_each(db.begin(), db.end(), 
                [&] (const std::string s) { s_out += s + "\n"; });
        // place into promise object
        m_promise.set_value(s_out);

    }
};

// thread creation and management
class ThreadManagement {
private:
    const int m_num_devices;
    const int m_num_data_proc;

public:
    constexpr ThreadManagement(const int num_devices, const int num_data_proc) :
        m_num_devices(num_devices), m_num_data_proc(num_data_proc) {};
    
    void operator()() {
        // shared
        device_q_t device_q;
        data_q_t data_q;
        std::atomic<int> num_device_threads(m_num_devices);
        std::atomic<int> num_data_threads(m_num_data_proc);
        std::promise<std::string> promise_obj;
        // local
        std::future<std::string> future_obj = promise_obj.get_future();
        std::vector<std::thread> threadListDevice;
        std::vector<std::thread> threadListData;

        // create threads
        // DeviceDataGenerator
        for (int i = 0; i < static_cast<int> (m_num_devices); i++) {
            threadListDevice.push_back(std::thread(DeviceDataGenerator(device_q,
                                num_device_threads, i)));
        }
        // DataProcessor
        for (int i = 0; i < m_num_data_proc; i++) {
            threadListData.push_back(std::thread(DataProcessor(device_q, data_q,
                                num_device_threads, num_data_threads, m_num_devices)));
        }

        // Database
        std::thread dbThread(Database(data_q, num_data_threads, promise_obj, m_num_devices));

        // join threads
        std::for_each(threadListDevice.begin(), threadListDevice.end(),
                      std::mem_fn(&std::thread::join));
        std::for_each(threadListData.begin(), threadListData.end(),
                      std::mem_fn(&std::thread::join));
        dbThread.join();

        // print Database report
        std::cout << future_obj.get() << std::endl;
    }
};


/*  constants  */
// number of DeviceDataGenerator threads
constexpr static unsigned int NUM_DEVICES = 20; // must be > 0
// number of DataProcessor threads
constexpr static unsigned int NUM_DATA_PROC = 5; // must be > 0

int main(int argc, char* argv[]) {
    int num_devices = NUM_DEVICES;
    int num_data_proc = NUM_DATA_PROC;

    if (argc != 1 && argc != 3) {
        std::cout << "usage: \n";
        std::cout << "  0 parameters: (default values)\n";
        std::cout << "  2 parameters: <number of devices>, ";
        std::cout << "<number of data processors> \n";

        return EXIT_FAILURE;
    }

    if (argc == 3) {
        num_devices = std::atoi(argv[1]);
        num_data_proc = std::atoi(argv[2]);
    }

    std::cout << "DeviceDataGenerator threads: " << num_devices << std::endl;
    std::cout << "DataProcessor threads: " << num_data_proc << std::endl;
    std::cout << "Database threads: " << 1 << std::endl << std::endl;

    std::cout << "Processing data...\n";

    ThreadManagement(num_devices, num_data_proc)();

    return EXIT_SUCCESS;
}
