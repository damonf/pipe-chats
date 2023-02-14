target_sources(pipe-chat
    PRIVATE
        pcclient.cpp
        input_poller.cpp
)

target_include_directories(pipe-chat PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(interface)
