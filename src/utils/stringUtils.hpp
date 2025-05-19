#pragma once

#include <string>
#include <cctype>
#include <algorithm>

#include "attributes.hpp"

void toLower(std::string *str);

void eraseWhiteSpace(std::string *str);

void eraseTabLFCR(std::string *str);

void eraseDoubleSpaces(std::string *str);

bool string_contains(const std::string &str1, const std::string &str2);

bool stringStartsWith(const std::string &str, const std::string &prefix);

void replaceOnce(std::string& str, std::string const& toReplace, std::string const& replaceWith);