set(TARGET sqlite3_helper)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../bin")

include_directories(${CMAKE_SOURCE_DIR}/sqlite3)

add_executable(${TARGET} sqlite3_helper_example.cpp sqlite3_helper.h)
target_link_libraries(${TARGET} sqlite3)
add_dependencies(${TARGET} sqlite3)

