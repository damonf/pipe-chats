#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "pcserver.h"
#include "factory_fake.h"
#include "iclient.h"

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp> 
#include <boost/di.hpp>
#include <boost/di/extension/injections/factory.hpp>

#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>
#include <utility>

using namespace pipe_chat;
using trompeloeil::_; // NOLINT

namespace sv = pipe_chat::svr;
//namespace it = sv::interface;

using sv::interface::IListener;
using sv::interface::IClient;

namespace pcserver_tests {

    //class ClientMock : public trompeloeil::mock_interface<IClient> {
    class ClientMock : public IClient {
    public:
        //static constexpr bool trompeloeil_movable_mock = true;
        MAKE_CONST_MOCK0(
                client_id, // NOLINT
                int(),
                override
                );

        MAKE_MOCK1(
                write, // NOLINT
                bool(const ClientMsg& ),
                override
                );
    };

    class ListenerMock : public IListener {
    public:
        MAKE_MOCK0(
                read, // NOLINT
                std::optional<ClientMsg>(),
                override
                );
    };

    using ClientMockWithId = std::pair<int, std::unique_ptr<ClientMock>>; 
    using ClientMockFactory = FactoryFake<ClientMock, IClient, ClientMockWithId>;

    TEST_CASE("pcserver tests", "[pcserver]")
    {
        SECTION("test new client joins chat") {

            auto listener_mock = std::make_unique<ListenerMock>();
            auto client_mock = std::make_unique<ClientMock>();

            auto& listener = *listener_mock;
            auto& client = *client_mock;

            const int client_id = 123;
            ClientMsg client_msg{ client_id, {} };
            client_msg.msg.fill('a');

            REQUIRE_CALL(listener, read())
                .RETURN(std::optional<ClientMsg>{client_msg});

            REQUIRE_CALL(client, client_id())
                .RETURN(client_id)
                .TIMES(AT_LEAST(1));

            auto factory = std::make_shared<ClientMockFactory>(
                    ClientMockWithId{client_id, std::move(client_mock)}
                    );

            sv::PCServer sut{
                std::move(listener_mock)
              , factory
            };

            sut.next_message();
        }

        SECTION("test two messages from one client, no writes") {

            auto listener_mock = std::make_unique<ListenerMock>();
            auto client_mock = std::make_unique<ClientMock>();

            auto& listener = *listener_mock;
            auto& client = *client_mock;

            const int client_id = 123;

            ClientMsg client_msg1{ client_id, {} };
            client_msg1.msg.fill('a');

            ClientMsg client_msg2{ client_id, {} };
            client_msg2.msg.fill('b');

            trompeloeil::sequence seq;

            REQUIRE_CALL(listener, read())
                .IN_SEQUENCE(seq)
                .RETURN(std::optional<ClientMsg>{client_msg1});

            REQUIRE_CALL(listener, read())
                .IN_SEQUENCE(seq)
                .RETURN(std::optional<ClientMsg>{client_msg2});

            REQUIRE_CALL(client, client_id())
                .RETURN(client_id)
                .TIMES(AT_LEAST(1));

            // no calls to write

            auto factory = std::make_shared<ClientMockFactory>(
                    ClientMockWithId{client_id, std::move(client_mock)}
                    );

            sv::PCServer sut{
                std::move(listener_mock)
              , factory
            };

            sut.next_message();
            sut.next_message();
        }

        SECTION("test two clients join chat, write to first client") {

            auto listener_mock = std::make_unique<ListenerMock>();
            auto client_mock1 = std::make_unique<ClientMock>();
            auto client_mock2 = std::make_unique<ClientMock>();

            auto& listener = *listener_mock;
            auto& client1 = *client_mock1;
            auto& client2 = *client_mock2;

            const int client_id1 = 111;
            ClientMsg client_msg1{ client_id1, {} };
            client_msg1.msg.fill('a');

            const int client_id2 = 222;
            ClientMsg client_msg2{ client_id2, {} };
            client_msg2.msg.fill('b');

            trompeloeil::sequence seq;

            REQUIRE_CALL(listener, read())
                .IN_SEQUENCE(seq)
                .RETURN(std::optional<ClientMsg>{client_msg1});

            REQUIRE_CALL(listener, read())
                .IN_SEQUENCE(seq)
                .RETURN(std::optional<ClientMsg>{client_msg2});

            REQUIRE_CALL(client1, client_id())
                .RETURN(client_id1)
                .TIMES(AT_LEAST(1));

            // client1 should get a message from client2
            REQUIRE_CALL(client1, write(_))
                .WITH(_1.client_id == 222)
                .RETURN(true);

            REQUIRE_CALL(client2, client_id())
                .RETURN(client_id2)
                .TIMES(AT_LEAST(1));

            auto factory = std::make_shared<ClientMockFactory>(
                    ClientMockWithId{client_id1, std::move(client_mock1)},
                    ClientMockWithId{client_id2, std::move(client_mock2)}
                    );

            sv::PCServer sut{
                std::move(listener_mock)
              , factory
            };

            sut.next_message();
            sut.next_message();
        }

        // 2nd client sends 2 messages
        // Writing first message to client1 fails, client1 removed
        // Second message will not be written to client1, since its gone
        SECTION("test failure writing to client") {

            auto listener_mock = std::make_unique<ListenerMock>();
            auto client_mock1 = std::make_unique<ClientMock>();
            auto client_mock2 = std::make_unique<ClientMock>();

            auto& listener = *listener_mock;
            auto& client1 = *client_mock1;
            auto& client2 = *client_mock2;

            const int client_id1 = 111;
            ClientMsg client_msg1{ client_id1, {} };
            client_msg1.msg.fill('a');

            const int client_id2 = 222;
            ClientMsg client_msg2{ client_id2, {} };
            client_msg2.msg.fill('b');

            trompeloeil::sequence seq;

            REQUIRE_CALL(listener, read())
                .IN_SEQUENCE(seq)
                .RETURN(std::optional<ClientMsg>{client_msg1});

            REQUIRE_CALL(listener, read())
                .IN_SEQUENCE(seq)
                .RETURN(std::optional<ClientMsg>{client_msg2});

            // second message from client 2
            REQUIRE_CALL(listener, read())
                .IN_SEQUENCE(seq)
                .RETURN(std::optional<ClientMsg>{client_msg2});

            REQUIRE_CALL(client1, client_id())
                .RETURN(client_id1)
                .TIMES(AT_LEAST(1));

            // client1 should get a message from client2
            REQUIRE_CALL(client1, write(_))
                .WITH(_1.client_id == 222)
                // return failure from the write
                .RETURN(false);

            REQUIRE_CALL(client2, client_id())
                .RETURN(client_id2)
                .TIMES(AT_LEAST(1));

            auto factory = std::make_shared<ClientMockFactory>(
                    ClientMockWithId{client_id1, std::move(client_mock1)},
                    ClientMockWithId{client_id2, std::move(client_mock2)}
                    );

            sv::PCServer sut{
                std::move(listener_mock)
              , factory
            };

            sut.next_message();
            sut.next_message();
            sut.next_message();
        }
    }

}
