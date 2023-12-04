#pragma once

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


template<typename T>
bool contains(const vector<T>& numbers, T number)
{
    return find(numbers.begin(), numbers.end(), number) != numbers.end();
}
