#pragma once

#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

// https://github.com/kmgb/PatternScan
void* scan_codestyle(void* start, uintptr_t length, uint8_t const* bytes, bool const* mask, std::size_t pat_len);
void* scan_idastyle(void* start, uintptr_t length, std::string const& pattern);