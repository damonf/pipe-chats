Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  #GIT_TAG        v3.0.1
  GIT_TAG        v2.13.10
  )

FetchContent_Declare(
  Trompeloeil
  GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
  GIT_TAG        v43
  )

FetchContent_MakeAvailable(Catch2 Trompeloeil)

include_directories(
  SYSTEM
  ${INCLUDE_BASE}
  ${trompeloeil_SOURCE_DIR}/include 
  ${catch2_SOURCE_DIR}/single_include
  )

add_executable(test-pipe-chat
  )

add_subdirectory(pipe-chat)

target_link_libraries(test-pipe-chat
  PRIVATE
  pipe-chat
  # catch2 3
  #Catch2::Catch2WithMain
  # catch2 2
  Catch2::Catch2
  trompeloeil::trompeloeil
  )

# catch2 2
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
# catch2 3
#list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

# Run the function defined by the Catch2 CMake module
catch_discover_tests(test-pipe-chat)

#message(STATUS "catch2_SOURCE_DIR=${catch2_SOURCE_DIR}")
#It's here: build/_deps/catch2-src

#message(STATUS "trompeloeil_SOURCE_DIR=${trompeloeil_SOURCE_DIR}")
#It's here: build/_deps/trompeloeil-src

