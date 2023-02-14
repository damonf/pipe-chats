// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef CLIENT_MSG_H
#define CLIENT_MSG_H

#include <array>
#include <algorithm>

#include <sys/types.h>
#include <climits>

namespace pipe_chat {

    struct ClientMsg {
        private:
            static constexpr uint msg_size = std::min(
                    (size_t)64,
                    PIPE_BUF - sizeof(pid_t)
                    );

        public:
            using Buffer = std::array<char, msg_size>;  

            pid_t client_id;
            Buffer msg;
    };

}

#endif // CLIENT_MSG_H
