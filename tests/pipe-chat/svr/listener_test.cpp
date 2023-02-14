#include "listener.h"
#include "inamed_pipe.h"
#include "client_msg.h"

#include <sys/types.h> // mode_t
#include <utility>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp> 
#include <boost/di.hpp>

using namespace pipe_chat;
using trompeloeil::_; // NOLINT

namespace sv = pipe_chat::svr;

using pipe_chat::INamedPipe;

namespace pcserver_tests {

    class NamedPipeMock : public INamedPipe<ClientMsg> {
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

    TEST_CASE("listener tests", "[listener]")
    {
        SECTION("test returns empty optional when read error ") {

            auto receiver_mock = std::make_unique<NamedPipeMock>();
            auto dummy_mock = std::make_unique<NamedPipeMock>();

            auto& receiver = *receiver_mock;
            auto& dummy = *dummy_mock;

            REQUIRE_CALL(receiver, create(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            REQUIRE_CALL(receiver, open(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            // read error
            REQUIRE_CALL(receiver, read(_))
                .RETURN(std::pair<ssize_t, int>{-1, 0});

            REQUIRE_CALL(dummy, open(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            sv::Listener sut{
                ServerPipeName{"server"}
              , std::move(receiver_mock)
              , std::move(dummy_mock)
            };

            auto result = sut.read();

            REQUIRE(result.has_value() == false);
        }

        SECTION("test returns empty optional when read incomplete") {

            auto receiver_mock = std::make_unique<NamedPipeMock>();
            auto dummy_mock = std::make_unique<NamedPipeMock>();

            auto& receiver = *receiver_mock;
            auto& dummy = *dummy_mock;

            REQUIRE_CALL(receiver, create(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            REQUIRE_CALL(receiver, open(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            // incomplete read
            REQUIRE_CALL(receiver, read(_))
                .RETURN(std::pair<ssize_t, int>{1, 0});

            REQUIRE_CALL(dummy, open(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            sv::Listener sut{
                ServerPipeName{"server"}
              , std::move(receiver_mock)
              , std::move(dummy_mock)
            };

            auto result = sut.read();

            REQUIRE(result.has_value() == false);
        }

        SECTION("test returns expected message when no read error") {

            auto receiver_mock = std::make_unique<NamedPipeMock>();
            auto dummy_mock = std::make_unique<NamedPipeMock>();

            auto& receiver = *receiver_mock;
            auto& dummy = *dummy_mock;

            REQUIRE_CALL(receiver, create(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            REQUIRE_CALL(receiver, open(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            REQUIRE_CALL(receiver, read(_))
                .RETURN(std::pair<ssize_t, int>{sizeof(ClientMsg), 0});

            REQUIRE_CALL(dummy, open(_,_))
                .RETURN(std::pair<bool, int>{true, 0});

            sv::Listener sut{
                ServerPipeName{"server"}
              , std::move(receiver_mock)
              , std::move(dummy_mock)
            };

            auto result = sut.read();

            REQUIRE(result.has_value() == true);
        }
    }
}

