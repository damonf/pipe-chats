target_sources(test-pipe-chat
    PRIVATE
      pcserver_test.cpp
      listener_test.cpp
      client_test.cpp
)

target_include_directories(test-pipe-chat PUBLIC ${CMAKE_CURRENT_LIST_DIR})
