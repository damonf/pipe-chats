// Copyright (c) Damon Friendship
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#ifndef INJECTOR_H
#define INJECTOR_H

#include "inamed_pipe.h"
#include "pcserver.h"
#include "pcclient.h"
#include "listener.h"
#include "iinput_poller.h"
#include "input_poller.h"

#include <boost/di.hpp>
#include <boost/di/extension/injections/factory.hpp>

namespace injector {

    namespace di = boost::di;
    namespace pc = pipe_chat;
    namespace sv = pc::svr;
    namespace cl = pc::clnt;
    namespace si = sv::interface;
    namespace ci = cl::interface;
    
    constexpr const char* SERVER_PIPE = "/tmp/pipe-chat-svr";
    constexpr const char* CLIENT_PIPE_PREFIX = "/tmp/pipe-chat-client";

    const auto base_module = [] {
        return di::make_injector(
    
                di::bind<pc::INamedPipe<pc::ClientMsg>>()
                    .to<pc::NamedPipe<pc::ClientMsg>>()
    
              , di::bind<pc::INamedPipe<pc::ClientMsg::Buffer>>()
                    .to<pc::NamedPipe<pc::ClientMsg::Buffer>>()
    
              , di::bind<pc::ServerPipeName>().to(pc::ServerPipeName{SERVER_PIPE})
    
              , di::bind<pc::ClientPipeNamePrefix>().to(pc::ClientPipeNamePrefix{CLIENT_PIPE_PREFIX})
        );
    };
    
    const auto svr_module = [] {
        return di::make_injector(
    
                di::bind<di::extension::ifactory<si::IClient, int>>()
                    .to(di::extension::factory<sv::Client>{}) 
    
              , di::bind<si::IListener>().to<sv::Listener>()
        );
    };
    
    const auto clnt_module = [] {
        return di::make_injector(
                di::bind<ci::IInputPoller>().to<cl::InputPoller>()
        );
    };
    
    inline auto make_client_injector() {
        return di::make_injector(base_module(), clnt_module());
    }
    
    inline auto make_server_injector() {
        return di::make_injector(base_module(), svr_module());
    }

}

#endif // INJECTOR_H
