// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#include "listener.h"

#include <sys/stat.h>
#include <fcntl.h>

#include <stdexcept>
#include <iostream>
#include <utility>
#include <cstring> // for strerror

namespace pipe_chat::svr {
    
    Listener::Listener(
            const ServerPipeName& server_pipe_name
          , std::unique_ptr<INamedPipe<ClientMsg>> receiver
          , std::unique_ptr<INamedPipe<ClientMsg>> dummy
            ) 
        :
        receiver_{std::move(receiver)}
      , dummy_{std::move(dummy)}
    {
        auto result = receiver_->create(server_pipe_name, permissions_);

        if (!result.first) {
            throw std::domain_error(strerror(result.second));
        }

        result = receiver_->open(server_pipe_name, O_RDONLY);

        if (!result.first) {
            throw std::domain_error(strerror(result.second));
        }

        // stops the pipe returning EOF if all clients close their end
        result = dummy_->open(server_pipe_name, O_WRONLY);

        if (!result.first) {
            throw std::domain_error(strerror(result.second));
        }
    }

    auto Listener::read() -> std::optional<ClientMsg> {

        ClientMsg client_msg{};

        auto result = receiver_->read(client_msg);

        if (result.first == -1) {
            std::cerr << "Error reading client message: "
                      << strerror(result.second)
                      << std::endl;
            return {};
        }

        if (result.first != sizeof(client_msg)) {
            std::cerr << "Incomplete client message was discarded :"
                << result.first << " bytes" << std::endl;
            return {};
        }

        std::cout << "Message from client with id: " << client_msg.client_id << std::endl;

        return client_msg;
    }

}
