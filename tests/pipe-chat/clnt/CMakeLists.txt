target_sources(test-pipe-chat
    PRIVATE
      pcclient_test.cpp
)

target_include_directories(test-pipe-chat PUBLIC ${CMAKE_CURRENT_LIST_DIR})

