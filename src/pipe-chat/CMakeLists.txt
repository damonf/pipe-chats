target_sources(pipe-chat
    PRIVATE
)

target_include_directories(pipe-chat PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(svr)
add_subdirectory(clnt)

