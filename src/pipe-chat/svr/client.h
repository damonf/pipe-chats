// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef CLIENT_H
#define CLIENT_H

#include "iclient.h"
#include "client_msg.h"
#include "named_pipe.h"
#include "client_pipe_name_prefix.h"

#include <string>
#include <memory>

namespace pipe_chat::svr {

    class Client : public interface::IClient {
        public:
            Client(
                    const ClientPipeNamePrefix& pipe_prefix
                  , int client_id
                  , std::unique_ptr<INamedPipe<ClientMsg::Buffer>> named_pipe
                  );

            Client(Client&& other) noexcept = default;
            auto operator=(Client&& other) noexcept -> Client& = default;

            Client(const Client&) = delete;
            auto operator=(const Client&) -> Client& = delete;

            ~Client() override = default;

            [[nodiscard]] auto client_id() const -> int override;
            [[nodiscard]] auto write(const ClientMsg& client_msg) -> bool override;

        private:
            auto open() -> bool;

            std::string pipe_prefix_;
            int client_id_;
            std::unique_ptr<INamedPipe<ClientMsg::Buffer>> pipe_{};
    };
}

#endif // CLIENT_H
