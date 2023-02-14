// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#include "pcserver.h"
#include "listener.h"

#include <csignal>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>

namespace pipe_chat::svr {

    PCServer::PCServer(
            std::unique_ptr<interface::IListener> listener
          , std::shared_ptr<di::extension::ifactory<interface::IClient, int>> client_factory 
            )
        : listener_{std::move(listener)}
        , client_factory_{std::move(client_factory)}
    {
    }

    void PCServer::run() {

        // Ignore SIGPIPE signal when writing to disconnected client FIFO
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
            throw std::domain_error(strerror(errno));
        }

        while (true) { // NOLINT 
            this->next_message();
        }
    }

    void PCServer::next_message() {

        auto client_msg = listener_->read();

        if (!client_msg.has_value()) {
            return;
        }

        if (clients_.find(client_msg->client_id) == clients_.end()) {

            std::cout << "adding new client with id "
                      << client_msg->client_id
                      << std::endl;

            auto client = client_factory_->create(
                    static_cast<int>(client_msg->client_id)
                    );

            clients_.insert({client_msg->client_id, std::move(client)});
        }

        for (auto it = clients_.begin(); it != clients_.end();) {

            auto& client = it->second;

            std::cout << "iterate client " << client->client_id() << std::endl;

            if (client->client_id() == client_msg->client_id) {
                ++it;
                continue;
            }

            auto success = client->write(*client_msg);

            if (success) {
                std::cout << "wrote to client " << std::endl;
                ++it;
            }
            else {
                it = clients_.erase(it);
                std::cout << "removed client " << std::endl;
            }
        }
    }

}
