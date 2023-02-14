
add_executable(pcserver
  "main.cpp"
  "handler.cpp"
  )

#include_directories(
    #"${INCLUDE_BASE}"
    #)

#find_package(Boost 1.79 COMPONENTS program_options REQUIRED)

include_directories(
  SYSTEM
  ${INCLUDE_BASE}
  )

target_link_libraries(pcserver PRIVATE pipe-chat)

#message(STATUS "BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}")
