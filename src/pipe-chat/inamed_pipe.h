// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef INAMED_PIPE_H
#define INAMED_PIPE_H

#include "client_msg.h"

#include <string>

namespace pipe_chat {

    template<class T>
    class INamedPipe {
        public:
            virtual ~INamedPipe() noexcept = default;

            INamedPipe() = default;
            INamedPipe(const INamedPipe&)     = delete;
            INamedPipe(INamedPipe&&) noexcept = delete;
            auto operator=(const INamedPipe&) -> INamedPipe& = delete;
            auto operator=(INamedPipe&&) noexcept -> INamedPipe& = delete;

            [[nodiscard]] virtual auto create(const std::string& name, mode_t mode) -> std::pair<bool, int> = 0;
            [[nodiscard]] virtual auto open(const std::string& name, int flags) -> std::pair<bool, int> = 0;
            [[nodiscard]] virtual auto is_open() const -> bool = 0;
            [[nodiscard]] virtual auto close() -> std::pair<bool, int> = 0;
            [[nodiscard]] virtual auto read(T& buffer) const -> std::pair<ssize_t, int> = 0;
            [[nodiscard]] virtual auto write(const T& buffer) const -> std::pair<ssize_t, int> = 0;
            [[nodiscard]] virtual auto fd() const -> int = 0;
    };
}

#endif // INAMED_PIPE_H


