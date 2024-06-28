#ifndef _CPP_LANG_HPP_
#define _CPP_LANG_HPP_

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <functional>
#include <memory>
#include <limits>
#include <thread>
#include <mutex>
#include <atomic>

// must be C++11 or later
#if __cplusplus < 201103
#   error "C++ is too old"
#endif  // __cplusplus < 201103

// macro for convienient namespace
#define BEGIN_NAMESPACE(x) namespace x {
#define END_NAMESPACE(x) }
#define USING_NAMESPACE(x) using namespace x

#endif // _CPP_LANG_HPP_
