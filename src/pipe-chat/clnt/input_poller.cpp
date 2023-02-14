// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#include "input_poller.h"

#include <array>
#include <stdexcept>
#include <cstring> // for strerror
                   //
#include <unistd.h>
#include <fcntl.h>

namespace pipe_chat::clnt {

    InputPoller::InputPoller()
        : epfd_{epoll_create1(0)} {

        if (epfd_ == -1) {
            throw std::logic_error(
                    std::string{"epoll_create error: "} +
                    strerror(errno)
                    );
        }
    }

    InputPoller::~InputPoller() {
        if (epfd_ != -1) {
            ::close(epfd_);
        }
    }

    InputPoller::InputPoller(InputPoller&& other) noexcept
        :
          epfd_{other.epfd_}
    {
        other.epfd_ = -1;
    }

    auto InputPoller::operator=(InputPoller&& other) noexcept -> InputPoller& {
        epfd_ = other.epfd_;
        other.epfd_ = -1;

        return *this;
    }

    void InputPoller::add(int fd) const {
        struct epoll_event ev{};

        ev.events = EPOLLIN;
        ev.data.fd = fd;

        if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
            throw std::logic_error(
                    std::string{"epoll_ctrl error: "} +
                    strerror(errno)
                    );
        }
    }

    auto InputPoller::wait() const -> std::pair<std::optional<interface::Event>, int> {

        const int SINGLE_EVENT = 1;
        std::array<struct epoll_event, SINGLE_EVENT> evlist{};
        
        auto ready = epoll_wait(epfd_, evlist.data(), SINGLE_EVENT, -1);

        if (ready == -1) {
            return std::make_pair(std::nullopt, errno);
        }

        return std::make_pair(
                interface::Event{ evlist[0].data.fd, evlist[0].events }
              , 0
                );
    }

}

