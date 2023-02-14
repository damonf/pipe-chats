// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef ICLIENT_H
#define ICLIENT_H

#include "client_msg.h"

namespace pipe_chat::svr::interface {

    class IClient {
        public:
            virtual ~IClient() noexcept = default;

            IClient() = default;
            IClient(const IClient&)     = delete;
            auto operator=(const IClient&) -> IClient& = delete;

        protected:
            IClient(IClient&&) noexcept = default;
            auto operator=(IClient&&) noexcept -> IClient& = default;

        public:

            [[nodiscard]] virtual auto client_id() const -> int = 0;
            [[nodiscard]] virtual auto write(const ClientMsg& client_msg) -> bool = 0;
    };
}

#endif // ICLIENT_H

