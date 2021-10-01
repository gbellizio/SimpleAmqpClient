#ifndef SIMPLEAMQPCLIENT_UTIL_H
#define SIMPLEAMQPCLIENT_UTIL_H
/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Copyright (c) 2010-2013 Alan Antonuk
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

#ifdef WIN32
#ifdef SimpleAmqpClient_EXPORTS
#define SIMPLEAMQPCLIENT_EXPORT __declspec(dllexport)
#else
#define SIMPLEAMQPCLIENT_EXPORT __declspec(dllimport)
#endif
#else
#define SIMPLEAMQPCLIENT_EXPORT
#endif

#if defined(__GNUC__) || defined(__clang__)
#define SAC_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define SAC_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#define SAC_DEPRECATED(msg)
#endif

#include <vector>
#include <string>

namespace AmqpClient {
namespace Detail {

/**
 * @brief c++11 replacement for boost::noncopyable
 */
class noncopyable {
 public:
  noncopyable() = default;
  noncopyable(const noncopyable&) = delete;
  const noncopyable& operator=(const noncopyable&) = delete;
};

/**
 * @brief Removes leading spaces from a string
 * @param str a string
 */
inline void ltrim(std::string &str) {
  str.erase(str.begin(), str.begin() + str.find_first_not_of(' '));
}

/**
 * @brief Removes trailing spaces from a string
 * @param str a string
 */
inline void rtrim(std::string &str) {
  auto it = str.rbegin();
  auto end = str.rend();

  while (it != end and *it == ' ') {
    ++it;
  }
  str.erase(it.base(), str.end());
}

/**
 * @brief Removes leading and trailing spaces from a string
 * @param str a string
 */
inline void trim(std::string &str) {
  rtrim(str);
  ltrim(str);
}

/**
 * @brief Simple boost::split replacement
 * @param tokens a container to fill with tokens
 * @param text a text
 * @param separators a collection of separators to tokenize @c str
 */
inline void split(std::vector<std::string> &tokens, const std::string &text,
                  const std::string &separators) {
  std::string token;

  for (auto c : text) {
    if (separators.find(c) != std::string::npos) {
      trim(token);
      if (!token.empty()) {
        tokens.push_back(token);
        token.clear();
      }
    } else {
      token += c;
    }
  }

  trim(token);

  if (!token.empty()) {
    if (!token.empty()) tokens.push_back(token);
  }
}

}
}
#endif  // SIMPLEAMQPCLIENT_UTIL_H
