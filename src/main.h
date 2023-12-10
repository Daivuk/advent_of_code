#pragma once

#include <atomic>
#include <cassert>
#include <chrono>
#include <future>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <onut/Async.h>
#include <onut/Files.h>
#include <onut/Dispatcher.h>
#include <onut/Point.h>
#include <onut/Crypto.h>
#include <onut/Strings.h>

using namespace std;
using namespace chrono_literals;


using duration_t = decltype(chrono::steady_clock::now() - chrono::steady_clock::now());


struct puzzle_t
{
    vector<string> data;
    int64_t result = -1;
    duration_t data_parsing_duration = 0s;
    duration_t duration = 0s;
    atomic<float> progress = 0.0f;
    int part = 0; // 1 or 2
    bool is_test = false;
    string data_filename;
    string name;
};


vector<int64_t> parse_numbers(const string& str);


template<typename T>
bool contains(const vector<T>& numbers, T number)
{
    return find(numbers.begin(), numbers.end(), number) != numbers.end();
}
