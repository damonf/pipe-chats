// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#include "pcserver.h"
#include "pcclient.h"
#include "injector.h"
#include "utils.h"
#include "handler.h"

#include <unistd.h> // for EXIT_FAILURE, EXIT_SUCCESS 
#include <sys/stat.h> // for S_IRUSR | S_IWUSR | S_IWGRP0 
#include <csignal>
#include <cstring>

#include <iostream>
#include <string>
#include <stdexcept>

#include <boost/di.hpp>
#include <boost/di/extension/injections/factory.hpp>

namespace {

    namespace pc = pipe_chat;
    namespace sv = pc::svr;
    namespace cl = pc::clnt;

}

auto main([[maybe_unused]] int argc, char *argv[]) -> int { // NOLINT

    try {
        std::string prog = argv[0]; // NOLINT

        if (utils::ends_with(prog, "pcclient")) {
            std::cout << "Client started ..." << std::endl;

            const auto injector = injector::make_client_injector();
            auto client = injector.create<cl::PCClient>();
            auto pipe_name = client.pipe_name();
            handler::install_handler(pipe_name);
            client.run();
        }
        else {
            std::cout << "Server started ..." << std::endl;

            auto pipe_name = pc::ServerPipeName{injector::SERVER_PIPE}; 
            handler::install_handler(pipe_name);
            const auto injector = injector::make_server_injector();
            auto server = injector.create<sv::PCServer>();
            server.run();
        }

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
}
