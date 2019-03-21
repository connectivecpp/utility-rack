/** @file
 *  
 *  @brief Example code demonstrating use of @c chops::repeat. 
 * 
 *  @author Thurman Gillespy
 * 
 *  Copyright (c)2019 by Thurman Gillespy
 *  3/17/19
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  Sample make file:
 *  g++ -std=c++17 -I ~/Projects/utility-rack/include/ repeatDemo.cpp
 */

#include <iostream>
#include <cstdlib>
#include "utility/repeat.hpp"

void printHello() { std::cout << "Hello, world\n"; } // simple function

void (*helloPtr)() = printHello; // pointer to simple function

void printNum(const int num) { std::cout << num << " "; } // print the number passed to function

// calculate factorial - each call calculates the next factorial
const void calcFactorial(const bool print) {
    static int count = 1;
    static u_int64_t fact = 1;

    fact *= count;
    
    if (print) {
        std::cout << count << "!= " << fact <<std::endl;
    }

    ++count;
}

// wrapper functions determine whether values are printed or not
const void advanceFactorial() { calcFactorial(false); }
const void printFactorial() { calcFactorial(true); }

// calculate factorial
class Factorial {
public:
    // default constructor (with 0!)
    constexpr Factorial() {};

    // construct with num!
    constexpr Factorial(const int num) { next(num); }

    // get the current calculated factorial (default is 0!)
    constexpr u_int64_t getFact() const { return fact; }

    constexpr int getCount() const { return count; }

    // calculate the next num factorials
    constexpr u_int64_t next(const int num) {
        chops::repeat(num, [&] () { fact *= (count++ == 0 ? 1 : count); } );
        return fact;
    }

    // calculate the next factorial
    constexpr u_int64_t next()  {
        return next(1);
    }

    // print methods
    void print() const { std::cout << fact <<std::endl; }
    void print(const char end) const { std::cout << fact << end; }

private:
    int count = 0;
    u_int64_t fact = 1;
};

// tasty utility lambdas
constexpr auto printLn = [] () { std::cout << std::endl; };
constexpr auto printStr = [] (std::string str) { std::cout << str << std::endl; };

int main() {
    
    printStr("calling chops::repeat with function, pointer to function, lambda function");

    chops::repeat(3, printHello); // use a function
    printLn();
    chops::repeat(2, helloPtr); // use pointer to function
    printLn();
    chops::repeat(7, [] () {std::cout << "Hello" << " ";}); // use lambda function
    printLn();
    // print varialble passed to function
    chops::repeat(5, printNum);
    printLn();

    // sum of the first n numbers 
    int sum = 0;
    int n = 10;
    chops::repeat(n, [&] () { static int i = 1; sum += i++; });
    std::cout << "sum of the first " << n << " numbers = " << sum << std::endl;
    printLn();

    // factorials using lambda function
    printStr("factorial using lambda function");

    u_int64_t fact = 1;
    int num = 5;
    chops::repeat(5, [&] () { static int count = 1; fact *= count++; });
    std::cout << num <<"! = " << fact <<std::endl << std::endl;

    // factorials using functions
    printStr("factorials using functions");

    printStr("print the first 10 factorials");
    chops::repeat(10, printFactorial);
    printStr("print factorials 15 - 20");
    chops::repeat(4, advanceFactorial);
    chops::repeat(6, printFactorial);
    printLn();

    // factorials using class
    printStr("factorials using class Factorial");

    Factorial f1; // 4!
    f1.next(); f1.next(); f1.next(); f1.next(); // call ::next explicitly
    std::cout << f1.getCount() << "! = " << f1.getFact() << std::endl;
    
    Factorial f2; // 10!
    chops::repeat(10, [&] () { f2.next(); }); // call ::next using chops::repeat
    std::cout << f2.getCount() << "! = " << f2.getFact() << std::endl;

    return EXIT_SUCCESS;
}