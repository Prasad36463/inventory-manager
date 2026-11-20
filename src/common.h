// common.h - shared input helper. Echoes typed input when stdin is piped so
// that captured console output looks like a real terminal session.
#pragma once
#include <iostream>
#include <string>
#include <cstdio>
#ifdef _WIN32
  #include <io.h>
  #define STDIN_IS_TERMINAL() _isatty(_fileno(stdin))
#else
  #include <unistd.h>
  #define STDIN_IS_TERMINAL() isatty(0)
#endif
#include <stdexcept>

inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    if (!std::getline(std::cin, s)) throw std::runtime_error("Input ended");
    if (!STDIN_IS_TERMINAL()) std::cout << s << "\n";
    return s;
}
inline int readInt(const std::string& prompt) {
    std::string s = readLine(prompt);
    try { size_t p; int v = std::stoi(s, &p); if (p != s.size()) throw 1; return v; }
    catch (...) { throw std::invalid_argument("'" + s + "' is not a valid whole number"); }
}
inline double readDouble(const std::string& prompt) {
    std::string s = readLine(prompt);
    try { size_t p; double v = std::stod(s, &p); if (p != s.size()) throw 1; return v; }
    catch (...) { throw std::invalid_argument("'" + s + "' is not a valid number"); }
}
