// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#include "pcclient.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstring> // for strerror
#include <sstream>

#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

namespace pipe_chat::clnt {

    PCClient::PCClient(
            const ServerPipeName& server_pipe_name
          , const ClientPipeNamePrefix& client_prefix
          , std::unique_ptr<interface::IInputPoller> input_poller
          , std::unique_ptr<INamedPipe<ClientMsg>> sender
          , std::unique_ptr<INamedPipe<ClientMsg::Buffer>> receiver
            ) 
        :
        client_id_{getpid()}
      , pipe_name_{client_prefix.value + std::to_string(client_id_)}
      , input_poller_{std::move(input_poller)}
      , sender_{std::move(sender)}
      , receiver_{std::move(receiver)}
    {
        auto result = receiver_->create(pipe_name_, permissions_);

        if (!result.first) {
            throw std::domain_error(strerror(result.second));
        }
        
        result = receiver_->open(pipe_name_, O_RDONLY | O_NONBLOCK);

        if (!result.first) {
            throw std::domain_error(strerror(result.second));
        }

        result = sender_->open(server_pipe_name, O_WRONLY);

        if (!result.first) {
            throw std::domain_error(strerror(result.second));
        }    
    }

    void PCClient::run() {

        input_poller_->add(STDIN_FILENO);
        input_poller_->add(receiver_->fd());

        std::ostringstream oss;
        oss << "client " << client_id_ << " joined";
        this->send_message(oss.str());

        while (true) { // NOLINT
            this->next_message();
        }
    }

    void PCClient::next_message() {
        auto result = input_poller_->wait();

        if (!result.first) {

            if (result.second == EINTR) {
                // restart after interrupt
                return;
            }

            throw std::domain_error(
                    std::string{"epoll_wait error: "} +
                    strerror(errno)
                    );
        }

        auto event = result.first;

        if (event->fd == STDIN_FILENO) {
            send_message();
        }
        else if (event->fd == receiver_->fd()) {

            if ((event->events & (EPOLLHUP | EPOLLERR)) != 0) {
                throw std::domain_error("Server closed connection");
            }

            receive_messages();
        }
    }

     auto PCClient::pipe_name() const -> const std::string& {
        return pipe_name_;
    }

    void PCClient::send_message() {
        std::string line;
        std::getline(std::cin, line);

        this->send_message(line);
    }

    void PCClient::send_message(const std::string& message) {

        ClientMsg client_msg{};

        client_msg.client_id = client_id_;
        auto len = std::min(message.size(), client_msg.msg.size());
        std::copy_n(message.cbegin(), len, client_msg.msg.begin()); 

        auto result = sender_->write(client_msg);

        if (result.first == -1) {
            throw std::domain_error(
                    std::string{"Error writing to server: "} +
                    strerror(result.second)
                    );
        }

        if (result.first != sizeof(client_msg)) {
            // If this happends, server is probably permanently out of sync
            throw std::domain_error(
                    "Incomplete write to server"
                    );
        }
    }

    void PCClient::receive_messages() {

        auto more = true;

        while (more) {

            // Read (non-blocking) from receiver
            ClientMsg::Buffer buffer;
            auto result = receiver_->read(buffer);

            if (result.first == -1) {

                switch (result.second) {

                    case EAGAIN:
                        {
                            more = false;
                            continue;
                        } break;

                    default: 
                        {
                            throw std::domain_error(strerror(result.second));
                        }

                }
            }

            if (result.first == 0) {
                throw std::domain_error("Server closed connection");
            }

            std::cout.write(buffer.begin(), result.first);
            std::cout << std::endl;
        }
    }

}
