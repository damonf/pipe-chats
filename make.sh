#!/bin/bash -e

usage()
{
  echo "usage: $0 [-r|-c|-t|-tt]"
  echo "builds the project"
  echo "where -r = rebuild, -c = generate compile_commands.json, -t = run tests, -tt = run failed tests"
}

compile_commands()
{
  echo "regenerating compile_commands.json"
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
  cp compile_commands.json ..
}

full_build()
{
  mkdir build
  cd build

  if [ $1 = "ninja" ]; then
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug 
  else
    cmake .. -DCMAKE_BUILD_TYPE=Debug 
    #cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
  fi

  cmake --build .
  compile_commands

  cp ./apps/pcserver ./apps/pcclient
}

build()
{
  cd build
  cmake --build .
  compile_commands

  cp ./apps/pcserver ./apps/pcclient
}

case "$#" in

  0 ) echo "building"
      if [ ! -d build ]; then
        echo "doing full build"
        full_build
      else
        build
      fi
      ;;

  1 ) 
      if [ $1 = -r ]; then

        echo "re-building"
        rm -rf build
        full_build

      elif [ $1 = -c ]; then

        cd build
        compile_commands

      elif [ $1 = -t ]; then

        echo "running tests"
        cd build
        ctest

      elif [ $1 = -tt ]; then

        echo "re-running failed tests"
        cd build
        ctest --rerun-failed --output-on-failure 

      elif [ $1 = -n ]; then

        echo "building with ninja"

        if [ ! -d build ]; then
          full_build "ninja"
        else
          build
        fi

      else

        usage
        exit 1

      fi
      ;;


  * ) echo "too many args"
      usage
      ;;

esac


