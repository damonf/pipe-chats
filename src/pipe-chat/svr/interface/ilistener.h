// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef ILISTENER_H
#define ILISTENER_H

#include "client_msg.h"

#include <optional>

namespace pipe_chat::svr::interface {

    class IListener {
        public:
            virtual ~IListener() noexcept = default;

            IListener() = default;
            IListener(const IListener&)     = delete;
            auto operator=(const IListener&) -> IListener& = delete;

        protected:
            IListener(IListener&&) noexcept = default;
            auto operator=(IListener&&) noexcept -> IListener& = default;

        public:
            virtual auto read() -> std::optional<ClientMsg> = 0;
    };
}

#endif // ILISTENER_H
