// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef PCCLIENT_H
#define PCCLIENT_H

#include "server_pipe_name.h"
#include "client_pipe_name_prefix.h"
#include "client_msg.h"
#include "iinput_poller.h"
#include "inamed_pipe.h"

#include <optional>
#include <memory>

#include <sys/stat.h>

namespace pipe_chat::clnt {

    class PCClient {
        public:
            PCClient(
                    const ServerPipeName& server_pipe_name
                  , const ClientPipeNamePrefix& client_prefix
                  , std::unique_ptr<interface::IInputPoller> input_poller
                  , std::unique_ptr<INamedPipe<ClientMsg>> sender
                  , std::unique_ptr<INamedPipe<ClientMsg::Buffer>> receiver
                    );

            void run();
            void next_message();
            [[nodiscard]] auto pipe_name() const -> const std::string&;

        private:
            void send_message();
            void send_message(const std::string& message);
            void receive_messages();

            pid_t client_id_{};
            std::string pipe_name_;
            std::unique_ptr<interface::IInputPoller> input_poller_;
            std::unique_ptr<INamedPipe<ClientMsg>> sender_;
            std::unique_ptr<INamedPipe<ClientMsg::Buffer>> receiver_;

            inline static constexpr int permissions_ = S_IRUSR | S_IWUSR | S_IWGRP;
    };

}

#endif // PCCLIENT_H
