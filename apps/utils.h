// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace utils {

    inline auto ends_with(std::string_view str, std::string_view suffix) -> bool
    {
        return str.size() >= suffix.size() &&
               0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
    }

}

#endif // UTILS_H
