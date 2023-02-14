target_sources(pipe-chat
    PRIVATE
        client.cpp
        listener.cpp
        pcserver.cpp
)

target_include_directories(pipe-chat PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(interface)

