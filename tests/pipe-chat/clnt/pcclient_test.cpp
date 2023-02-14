#include "pcclient.h"

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp> 
#include <boost/di.hpp>

#include <iostream>
#include <utility>
#include <optional>
#include <algorithm>

#include <unistd.h>

using namespace pipe_chat;
using trompeloeil::_; // NOLINT

namespace clnt = pipe_chat::clnt;

using clnt::interface::IInputPoller;
using clnt::interface::Event;

namespace pcclient_tests {

    class InputPollerMock : public IInputPoller {
        public:
            MAKE_CONST_MOCK1(
                    add, // NOLINT
                    void(int ),
                    override
                    );

            MAKE_CONST_MOCK0(
                    wait, // NOLINT
                    (std::pair<std::optional<Event>, int>()),
                    override
                    );
    };

    class SenderMock : public INamedPipe<ClientMsg> {
        public:
        MAKE_MOCK2(
                create, // NOLINT
                (std::pair<bool, int>(const std::string&, mode_t)),
                override
                );

        MAKE_MOCK2(
                open, // NOLINT
                (std::pair<bool, int>(const std::string&, int)),
                override
                );

        MAKE_CONST_MOCK0(
                is_open, // NOLINT
                (bool()),
                override
                );

        MAKE_MOCK0(
                close, // NOLINT
                (std::pair<bool, int>()),
                override
                );

        MAKE_CONST_MOCK1(
                read, // NOLINT
                (std::pair<ssize_t, int>(ClientMsg&)),
                override
                );

        MAKE_CONST_MOCK1(
                write, // NOLINT
                (std::pair<ssize_t, int>(const ClientMsg&)),
                override
                );

        MAKE_CONST_MOCK0(
                fd, // NOLINT
                (int()),
                override
                );
    };

    class ReceiverMock : public INamedPipe<ClientMsg::Buffer> {
        public:
        MAKE_MOCK2(
                create, // NOLINT
                (std::pair<bool, int>(const std::string&, mode_t)),
                override
                );

        MAKE_MOCK2(
                open, // NOLINT
                (std::pair<bool, int>(const std::string&, int)),
                override
                );

        MAKE_CONST_MOCK0(
                is_open, // NOLINT
                (bool()),
                override
                );

        MAKE_MOCK0(
                close, // NOLINT
                (std::pair<bool, int>()),
                override
                );

        MAKE_CONST_MOCK1(
                read, // NOLINT
                (std::pair<ssize_t, int>(ClientMsg::Buffer&)),
                override
                );

        MAKE_CONST_MOCK1(
                write, // NOLINT
                (std::pair<ssize_t, int>(const ClientMsg::Buffer&)),
                override
                );

        MAKE_CONST_MOCK0(
                fd, // NOLINT
                (int()),
                override
                );
    };

    TEST_CASE("pcclient tests", "[pcclient]")
    {

        SECTION("test next_message writes a message from stdin to sender pipe") {

            const ClientPipeNamePrefix client_name_prefix {
                "client_name_prefix"
            };

            const ServerPipeName server_pipe_name {
                "server_pipe_name"
            };

            // setup calls for ctor
            auto receiver_mock = std::make_unique<ReceiverMock>();

            auto& receiver = *receiver_mock;

            REQUIRE_CALL(receiver, create(_, _))
                .RETURN(std::pair<bool, int>{true, 0});
            
            REQUIRE_CALL(receiver, open(_, _))
                .RETURN(std::pair<bool, int>{true, 0});

            auto sender_mock = std::make_unique<SenderMock>();

            auto& sender = *sender_mock;

            REQUIRE_CALL(sender, open(_, _))
                .RETURN(std::pair<bool, int>{true, 0});

            // setup calls for next_message
            auto input_poller_mock = std::make_unique<InputPollerMock>();

            auto& input_poller = *input_poller_mock;

            REQUIRE_CALL(input_poller, wait())
                .RETURN(std::pair<std::optional<Event>, int>{
                        Event{STDIN_FILENO, 0}, 0
                        });
            
            // setup calls for send_message
            const std::string test_message{"test message"};

            const std::istringstream sin{test_message};
            auto* cin_buffer = std::cin.rdbuf(sin.rdbuf());

            ClientMsg::Buffer buffer{};
            std::copy_n(test_message.cbegin(), test_message.length(), buffer.begin()); 

            REQUIRE_CALL(sender, write(_))
                .WITH(_1.msg == buffer) 
                .RETURN(std::pair<ssize_t, int>{sizeof(ClientMsg), 0});

            // call sut
            clnt::PCClient sut{
                server_pipe_name
              , client_name_prefix
              , std::move(input_poller_mock)
              , std::move(sender_mock)
              , std::move(receiver_mock)
            };

            sut.next_message();

            // restore cin buffer
            std::cin.rdbuf(cin_buffer);
        }

        SECTION("test next_message writes a message from receiver pipe to stdout") {

            const ClientPipeNamePrefix client_name_prefix {
                "client_name_prefix"
            };

            const ServerPipeName server_pipe_name {
                "server_pipe_name"
            };

            // setup calls for ctor
            auto receiver_mock = std::make_unique<ReceiverMock>();

            auto& receiver = *receiver_mock;

            REQUIRE_CALL(receiver, create(_, _))
                .RETURN(std::pair<bool, int>{true, 0});

            REQUIRE_CALL(receiver, open(_, _))
                .RETURN(std::pair<bool, int>{true, 0});

            auto sender_mock = std::make_unique<SenderMock>();

            auto& sender = *sender_mock;

            REQUIRE_CALL(sender, open(_, _))
                .RETURN(std::pair<bool, int>{true, 0});

            // setup calls for next_message
            auto input_poller_mock = std::make_unique<InputPollerMock>();

            auto& input_poller = *input_poller_mock;

            const auto receiver_fd = 4;
            REQUIRE_CALL(input_poller, wait())
                .RETURN(std::pair<std::optional<Event>, int>{
                        Event{receiver_fd, 0}, 0
                        });

            REQUIRE_CALL(receiver, fd())
                .RETURN(receiver_fd);

            // setup calls receive_messages

            // redirect stdout
            const std::ostringstream sout;
            auto* cout_rdbuf = std::cout.rdbuf(sout.rdbuf());

            std::string test_message{"test message"};

            // first call to receiver_->read fills buffer with data
            trompeloeil::sequence seq;

            REQUIRE_CALL(receiver, read(_))
                .IN_SEQUENCE(seq)
                .LR_SIDE_EFFECT(std::copy_n(test_message.cbegin(), test_message.length(), _1.begin()))
                .LR_RETURN(std::pair<ssize_t, int>{test_message.length(), 0});

            // second call to receiver_->read, no more data
            REQUIRE_CALL(receiver, read(_))
                .IN_SEQUENCE(seq)
                .RETURN(std::pair<ssize_t, int>{-1, EAGAIN});

            // call sut
            clnt::PCClient sut{
                server_pipe_name
                    , client_name_prefix
                    , std::move(input_poller_mock)
                    , std::move(sender_mock)
                    , std::move(receiver_mock)
            };

            sut.next_message();

            auto sent = sout.str();
            REQUIRE(std::equal(test_message.cbegin(), test_message.cend(), sent.cbegin()));

            // restore cout buffer
            std::cout.rdbuf(cout_rdbuf);
        }
    }

}

