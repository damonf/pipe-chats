// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef PCSERVER_H
#define PCSERVER_H

#include "ilistener.h"
#include "client.h"

#include <map>
#include <memory>
#include <iostream>

#include <boost/di.hpp>
#include <boost/di/extension/injections/factory.hpp>

namespace pipe_chat::svr {

    namespace di = boost::di;

    class PCServer {
        public:
            PCServer(
                    std::unique_ptr<interface::IListener> listener
                  , std::shared_ptr<di::extension::ifactory<interface::IClient, int>> client_factory 
                  //, std::shared_ptr<interface::IClientFactory> client_factory 
                    );
            void run();
            void next_message();

        private:
            std::unique_ptr<interface::IListener> listener_;
            std::shared_ptr<di::extension::ifactory<interface::IClient, int>> client_factory_;
            //std::shared_ptr<interface::IClientFactory> client_factory_;
            std::map<int, std::unique_ptr<interface::IClient>> clients_{};
    };
}

#endif // PCSERVER_H
