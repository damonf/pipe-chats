// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef NAMED_PIPE_H
#define NAMED_PIPE_H

#include "inamed_pipe.h"

#include <sys/types.h>

#include <utility>
#include <string>
#include <type_traits>
#include <stdexcept>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace pipe_chat {

    template<class T>
    class NamedPipe : public INamedPipe<T> {
        public:
            NamedPipe() = default;
            NamedPipe(NamedPipe&& other) noexcept;
            NamedPipe(const NamedPipe&) = delete;

            ~NamedPipe() override;

            auto operator=(NamedPipe&& other) noexcept -> NamedPipe&;
            auto operator=(const NamedPipe&) -> NamedPipe& = delete;

            [[nodiscard]] auto create(const std::string& name, mode_t mode) -> std::pair<bool, int> override;
            [[nodiscard]] auto open(const std::string& name, int flags) -> std::pair<bool, int> override;
            [[nodiscard]] auto is_open() const -> bool override;
            [[nodiscard]] auto close() -> std::pair<bool, int> override;
            [[nodiscard]] auto read(T& buffer) const -> std::pair<ssize_t, int> override;
            [[nodiscard]] auto write(const T& buffer) const -> std::pair<ssize_t, int> override;
            [[nodiscard]] auto fd() const -> int override;

        private:
            int fd_ = -1;
    };

    template<class T>
    NamedPipe<T>::NamedPipe(NamedPipe&& other) noexcept
        :
          fd_{other.fd_}
    {
        other.fd_ = -1;
    }

    template<class T>
    auto NamedPipe<T>::operator=(NamedPipe&& other) noexcept -> NamedPipe& {
        fd_ = other.fd_;
        other.fd_ = -1;

        return *this;
    }

    template<class T>
    NamedPipe<T>::~NamedPipe() {
        if (fd_ != -1) {
            ::close(fd_);
        }
    }

    template<class T>
    auto NamedPipe<T>::create(const std::string& name, mode_t mode) -> std::pair<bool, int> {
        if (mkfifo(name.c_str(), mode) == -1 && errno != EEXIST) {
            return std::make_pair(false, errno);
        }

        return std::make_pair(true, 0);
    }

    template<class T>
    auto NamedPipe<T>::open(const std::string& name, int flags) -> std::pair<bool, int> {

        if (fd_ != -1) {
            throw std::domain_error("pipe already opened");
        }

        fd_ = ::open(name.c_str(), flags); //NOLINT

        if (fd_ == -1) {
            return std::make_pair(false, errno); 
        }

        return std::make_pair(true, 0);
    }

    template<class T>
    auto NamedPipe<T>::is_open() const -> bool {
        return fd_ != -1;
    }

    template<class T>
    auto NamedPipe<T>::close() -> std::pair<bool, int> {

        if (::close(fd_) == -1) {
            return std::make_pair(false, errno); 
        }

        fd_ = -1;

        return std::make_pair(true, 0); 
    }

    template<class T>
    auto NamedPipe<T>::read(T& buffer) const -> std::pair<ssize_t, int> {

        static_assert(
                std::is_pod_v<T>,
                "Parameter 'buffer' is not a POD type"
                );

        auto bytes_read = ::read(fd_, &buffer, sizeof(T));

        if (bytes_read == -1) {
            return std::make_pair(-1, errno); 
        }

        return std::make_pair(bytes_read, 0);
    }

    template<class T>
    auto NamedPipe<T>::write(const T& buffer) const -> std::pair<ssize_t, int> {

        static_assert(
                std::is_pod_v<T>,
                "Parameter 'buffer' is not a POD type"
                );

        auto bytes_written = ::write(fd_, &buffer, sizeof(T));

        if (bytes_written == -1) {
            return std::make_pair(-1, errno); 
        }

        return std::make_pair(bytes_written, 0);
    }

    template<class T>
    auto NamedPipe<T>::fd() const -> int {
        return fd_;
    }

}

#endif // NAMED_PIPE_H
