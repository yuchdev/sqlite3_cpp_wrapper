# SQlite3 Helper

Very lifgtweight header-only C++ RAII wrapper under SQlite3 ANSI C API. The header itself could be found in `sqlite3_helper` directory with the usage example.

C++ wrapper does not provide and thread safety, instead it relies on thread safety of SQLite3 (see `sqlite3_helper::is_threadsafe()` method)

Class does not throw exceptions. It could be considered as not C++ way, however it increases safety in low-level application like drivers

SQlite3 source code itself included in the repo so that compile in one click. Cmake is required for the build, just create somethong like `build-cmake` directory, perform `cd build-cmake` and create build toolchain by `cmake ..` command