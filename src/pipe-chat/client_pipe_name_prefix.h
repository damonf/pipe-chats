// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef CLIENT_PIPE_NAME_PREFIX
#define CLIENT_PIPE_NAME_PREFIX

#include <string>

namespace pipe_chat {

    struct ClientPipeNamePrefix {
        std::string value; //NOLINT
        operator std::string() const { return value; }
    };
    
}
 
#endif // CLIENT_PIPE_NAME_PREFIX

