// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef SERVER_PIPE_NAME
#define SERVER_PIPE_NAME

#include <string>

namespace pipe_chat {

    struct ServerPipeName {
        std::string value; //NOLINT
        operator std::string() const { return value; }
    };
 
}

#endif // SERVER_PIPE_NAME
