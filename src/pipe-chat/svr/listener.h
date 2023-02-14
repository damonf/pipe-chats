// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef LISTENER_H
#define LISTENER_H

#include "server_pipe_name.h"
#include "ilistener.h"
#include "client_msg.h"
#include "inamed_pipe.h"

#include <sys/stat.h>

#include <optional>
#include <memory>

namespace pipe_chat::svr {

    class Listener : public interface::IListener {
        public:
            Listener(
                    const ServerPipeName& server_pipe_name
                  , std::unique_ptr<INamedPipe<ClientMsg>> receiver
                  , std::unique_ptr<INamedPipe<ClientMsg>> dummy
                    );

            Listener(Listener&& other) noexcept = default;
            auto operator=(Listener&& other) noexcept -> Listener& = default;

            Listener(const Listener&) = delete;
            auto operator=(const Listener&) -> Listener& = delete;

            ~Listener() override = default;

            auto read() -> std::optional<ClientMsg> override;

        private:
            std::unique_ptr<INamedPipe<ClientMsg>> receiver_;
            std::unique_ptr<INamedPipe<ClientMsg>> dummy_;

            inline static constexpr int permissions_ = S_IRUSR | S_IWUSR | S_IWGRP;
    };
}

#endif // LISTENER_H

