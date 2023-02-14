// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef INPUTPOLLER_H
#define INPUTPOLLER_H

#include "iinput_poller.h"

#include <utility>
#include <optional>

namespace pipe_chat::clnt {

    class InputPoller: public interface::IInputPoller {
        public:
            InputPoller();

            InputPoller(InputPoller&& other) noexcept;
            auto operator=(InputPoller&& other) noexcept -> InputPoller&;

            InputPoller(const InputPoller&) = delete;
            auto operator=(const InputPoller&) -> InputPoller& = delete;

            ~InputPoller() override;

            void add(int fd) const override;
            [[nodiscard]] auto wait() const -> std::pair<std::optional<interface::Event>, int> override;

        private:
            int epfd_{};
    };

}

#endif // INPUTPOLLER_H
