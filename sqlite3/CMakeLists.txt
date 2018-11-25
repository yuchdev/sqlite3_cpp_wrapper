set(TARGET sqlite3)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../bin")

#add_library(${TARGET} SHARED shell.c sqlite3.c sqlite3.h sqlite3ext.h)
add_library(${TARGET} shell.c sqlite3.c sqlite3.h sqlite3ext.h)
