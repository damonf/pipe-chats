// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#include "handler.h"

#include <string>
#include <stdexcept>
#include <unistd.h>

#include <csignal>
#include <cstring>

namespace handler {

    namespace {
        std::string pipe_name; // NOLINT

        void handler([[maybe_unused]] int sig) {
        
            if (!pipe_name.empty()) {
                unlink(pipe_name.c_str());
                pipe_name.clear();
            }
        
            _exit(0);
        }

        void call_handler() {
            handler(0);
        }
    }
    
    void install_handler(const std::string& pipename) {

        pipe_name = pipename;
    
        struct sigaction sa{};
    
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = handler;
    
        if (sigaction(SIGINT, &sa, nullptr) == -1) {
            throw std::domain_error(strerror(errno));
        }

        if (atexit(call_handler) != 0) {
            throw std::domain_error("Could not set atexit");
        }
    }

}
