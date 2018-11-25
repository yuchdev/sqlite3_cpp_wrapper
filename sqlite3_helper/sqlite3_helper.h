#include <sqlite3.h>

typedef int(*sqlite3_callback)(void*, int, char**, char**);

/// @briefVery lightweight header-only C++ RAII wrapper under SQlite3 ANSI C API
/// Class does not throw exceptions. It could be considered as not C++ way, 
/// however it increases safety in low-level application like drivers
class sqlite3_helper
{
public:

    /// @brief Empty-state sqlite3
    sqlite3_helper()
    {}

    /// @brief Open or create sqlite3 database
    sqlite3_helper(const char* database_name) :
        current_return_code_(sqlite3_open(database_name, &db_))
    {
    }

    /// @brief Close database handle
    /// Important, if sqlite3_close() in close() method returned error, database remain opened
    /// This situation may occur if database is under backup right now.
    /// We neglect this situation, as we destroy the database object, we don't need it anymore
    ~sqlite3_helper()
    {
        close();
    }

    /// No copy
    sqlite3_helper(const sqlite3_helper&) = delete;

    /// No assignment
    sqlite3_helper& operator=(const sqlite3_helper&) = delete;

    /// @brief Move c-tor leaves rhs-object in empty state
    /// without closing the database handle
    sqlite3_helper(sqlite3_helper&& rhs) :
        db_(rhs.db_),
        current_return_code_(rhs.current_return_code_)
    {
        db_ = rhs.db_;
        current_return_code_ = rhs.current_return_code_;
    }

    /// @brief Assignment operator leaves rhs-object in empty state
    /// without closing the database handle
    sqlite3_helper& operator=(sqlite3_helper&& rhs)
    {
        db_ = rhs.db_;
        current_return_code_ = rhs.current_return_code_;
        rhs.db_ = nullptr;
        rhs.current_return_code_ = 0;
    }


    /// @brief Open or create sqlite3 database
    /// @return: SQLite error code
    /// See https://www.sqlite.org/rescode.html for details
    int open(const char* database_name)
    {
        current_return_code_ = sqlite3_open(database_name, &db_);
    }

    /// @brief Close database handle
    /// If sqlite3_close() in close() method returned error, database remain opened
    /// It could be checked with get_last_error() and handle by the caller, 
    /// rather than look for solution inside the method. 
    /// Helper designed to be as lightweight as possible
    /// @return: SQLite error code
    /// See https://www.sqlite.org/rescode.html for details
    int close()
    {
        current_return_code_ = sqlite3_close(db_);
        if (current_return_code_ == SQLITE_OK) {
            db_ = nullptr;
        }
        return current_return_code_;
    }

    /// @brief Execute SQL query
    /// @return: SQLite error code
    /// See https://www.sqlite.org/rescode.html for details
    int exec(const char* sql, sqlite3_callback callback = nullptr)
    {
        current_return_code_ = sqlite3_exec(db_, sql, callback, nullptr, nullptr);
        return current_return_code_;
    }

    /// @brief Is database in valid state
    operator bool() const
    {
        return is_valid(); 
    }

    /// @brief Is database in valid state (handle is opened and last status is SQLITE_OK)
    bool is_valid() const
    {
        return (db_ != nullptr) && (current_return_code_ == SQLITE_OK);
    }

    /// @brief Check if the current sqlite3 build is thread-safe
    static bool is_threadsafe()
    {
        return sqlite3_threadsafe();
    }

    /// @brief Return last error code of any SQLite operation
    /// If the code is not SQLITE_OK, helper is not in valid state
    int get_last_error() 
    { 
        return current_return_code_; 
    }

    /// @brief Return last error message based on error code 
    const char* get_last_error_message() 
    {
        return sqlite3_errstr(current_return_code_);
    }

private:

    /// SQLite3 Handle
    sqlite3* db_ = nullptr;

    /// Last returned error code
    int current_return_code_ = SQLITE_OK;
};
