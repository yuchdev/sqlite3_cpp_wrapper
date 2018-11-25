cmake_minimum_required(VERSION 3.0)
project(sqlite_helper)

set(TARGET sqlite_helper)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message("UNIX congiguration, Clang, enable C++14")
    set(CMAKE_CXX_FLAGS "-std=gnu++14 -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu++14")

elseif(CMAKE_COMPILER_IS_GNUCC)
    message("UNIX congiguration, GCC, enable C++14, all warnings")
    # NOTE! gnu++1y is GCC 4.9 compiler option, in 5 it is deprectaed
    set(CMAKE_CXX_FLAGS "-std=c++11 -std=gnu++14 -Wall -Wextra -Wsign-conversion -pthread -fPIC")
    set(CMAKE_EXE_LINKER_FLAGS "-std=c++11 -std=gnu++14 -pthread")

elseif(WIN32)
    message("Windows configuraion: enable all exceptions, all warnings")
    set(MY_BOOST_DIR ${WINDOWS_BOOST_DIR})
    set(CMAKE_CXX_FLAGS "/EHa /W1 /MP")    
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

    # define WIN32 macro version
    macro(get_WIN32_WINNT version)
        if (WIN32 AND CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REPLACE "." "" ver ${ver})
            string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})

            set(${version} "0x${ver}")
        endif()
    endmacro()

    message("Set WINVER: " ver)
    message("Set SCL_SECURE_NO_WARNINGS")
    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)

    # Use folders to sort out projects in VS solution
    SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

endif()

message("Configurate system-dependent directories")

if(UNIX)
    find_package(Threads REQUIRED)
endif()

add_subdirectory(sqlite3)
add_subdirectory(sqlite3_helper)
