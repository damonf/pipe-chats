// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef IINPUT_POLLER_H
#define IINPUT_POLLER_H

#include <sys/epoll.h>
#include <utility>
#include <optional>

namespace pipe_chat::clnt::interface {

    struct Event {
        int fd;
        uint32_t events;
    };

    class IInputPoller {
        public:
            virtual ~IInputPoller() noexcept = default;

            IInputPoller() = default;
            IInputPoller(const IInputPoller&)     = delete;
            IInputPoller(IInputPoller&&) noexcept = delete;
            auto operator=(const IInputPoller&) -> IInputPoller& = delete;
            auto operator=(IInputPoller&&) noexcept -> IInputPoller& = delete;

            virtual void add(int fd) const = 0;
            [[nodiscard]] virtual auto wait() const -> std::pair<std::optional<Event>, int> = 0;
    };
}

#endif // IINPUT_POLLER_H


