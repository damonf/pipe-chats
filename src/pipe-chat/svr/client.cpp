// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#include "client.h"

#include <fcntl.h>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <cstring> // for strerror

namespace pipe_chat::svr {

    Client::Client(
            const ClientPipeNamePrefix& pipe_prefix
          , int client_id
          , std::unique_ptr<INamedPipe<ClientMsg::Buffer>> named_pipe
            ) 
        :
        pipe_prefix_{pipe_prefix}
      , client_id_{client_id}
      , pipe_(std::move(named_pipe))
    {
    }

    auto Client::client_id() const -> int {
        return client_id_;
    }

    auto Client::write(const ClientMsg& client_msg) -> bool {

        if (!pipe_->is_open() && !open()) {
            return false;
        }

        auto result = pipe_->write(client_msg.msg); 

        if (result.first == -1) {
            std::cerr << "Error writing to client pipe: "
                      << strerror(result.second)
                      << std::endl;
            return false;
        }

        if (result.first != sizeof(client_msg.msg)) {
            std::cerr << "Incomplete client message sent" << std::endl;
            return false;
        }

        return true;
    }

    auto Client::open() -> bool {

        auto pipe_name = pipe_prefix_ + std::to_string(client_id_);

        auto result = pipe_->open(pipe_name, O_WRONLY | O_NONBLOCK);

        if (!result.first) {
            std::cerr << "Error opening client pipe: "
                      << strerror(result.second)
                      << std::endl;
            return false;
        }

        return true;
    }

}
