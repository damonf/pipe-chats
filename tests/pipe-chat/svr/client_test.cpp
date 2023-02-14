#include "client.h"
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

    class NamedPipeMock : public INamedPipe<ClientMsg::Buffer> {
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

    TEST_CASE("client tests", "[client]")
    {
        SECTION("test returns false on error opening pipe") {

            auto named_pipe_mock = std::make_unique<NamedPipeMock>();

            auto& named_pipe = *named_pipe_mock;

            REQUIRE_CALL(named_pipe, is_open())
                .RETURN(false);

            // open fails
            REQUIRE_CALL(named_pipe, open(_, _))
                .RETURN(std::pair<bool, int>{false, 0});

            const int client_id = 123;
            ClientMsg client_msg{ client_id, {} };
            client_msg.msg.fill('a');

            sv::Client sut{
                ClientPipeNamePrefix{"prefix"},
                client_id,
                std::move(named_pipe_mock)
            };
            
            auto result = sut.write(client_msg);

            REQUIRE(result == false);
        }

        SECTION("test returns false on error writing to pipe") {

            auto named_pipe_mock = std::make_unique<NamedPipeMock>();

            auto& named_pipe = *named_pipe_mock;

            REQUIRE_CALL(named_pipe, is_open())
                .RETURN(true);

            // write error
            REQUIRE_CALL(named_pipe, write(_))
                .RETURN(std::pair<ssize_t, int>{-1, 0});

            const int client_id = 123;
            ClientMsg client_msg{ client_id, {} };
            client_msg.msg.fill('a');

            sv::Client sut{
                ClientPipeNamePrefix{"prefix"},
                client_id,
                std::move(named_pipe_mock)
            };
            
            auto result = sut.write(client_msg);

            REQUIRE(result == false);
        }

        SECTION("test returns false on partial write to pipe") {

            auto named_pipe_mock = std::make_unique<NamedPipeMock>();

            auto& named_pipe = *named_pipe_mock;

            REQUIRE_CALL(named_pipe, is_open())
                .RETURN(true);

            const int client_id = 123;
            ClientMsg client_msg{ client_id, {} };
            client_msg.msg.fill('a');

            // partial write
            REQUIRE_CALL(named_pipe, write(_))
                .RETURN(std::pair<ssize_t, int>{sizeof(client_msg.msg) - 1, 0});

            sv::Client sut{
                ClientPipeNamePrefix{"prefix"},
                client_id,
                std::move(named_pipe_mock)
            };
            
            auto result = sut.write(client_msg);

            REQUIRE(result == false);
        }

        SECTION("test returns true on successful write") {

            auto named_pipe_mock = std::make_unique<NamedPipeMock>();

            auto& named_pipe = *named_pipe_mock;

            REQUIRE_CALL(named_pipe, is_open())
                .RETURN(true);

            const int client_id = 123;
            ClientMsg client_msg{ client_id, {} };
            client_msg.msg.fill('a');

            REQUIRE_CALL(named_pipe, write(_))
                .RETURN(std::pair<ssize_t, int>{sizeof(client_msg.msg), 0});

            sv::Client sut{
                ClientPipeNamePrefix{"prefix"},
                client_id,
                std::move(named_pipe_mock)
            };
            
            auto result = sut.write(client_msg);

            REQUIRE(result == true);
        }
    }
}
