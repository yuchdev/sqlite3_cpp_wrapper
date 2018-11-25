#include <sqlite3.h>


class sqlite3_helper
{
public:

    sqlite3_helper()
    {}

    sqlite3_helper(const char* database_name) :
        current_return_code_(sqlite3_open(database_name, &db_))
    {
    }

    ~sqlite3_helper()
    {
        close();
    }

    int open(const char* database_name)
    {
        current_return_code_ = sqlite3_open(database_name, &db_);
    }

    int close()
    {
        current_return_code_ = sqlite3_close(db_);
        if (current_return_code_ == SQLITE_OK) {
            db_ = nullptr;
        }
        return current_return_code_;
    }

    sqlite3_helper(const sqlite3_helper&) = delete;
    sqlite3_helper& operator=(const sqlite3_helper&) = delete;

    sqlite3_helper(sqlite3_helper&& rhs) :
        db_(rhs.db_),
        current_return_code_(rhs.current_return_code_)
    {
        db_ = rhs.db_;
        current_return_code_ = rhs.current_return_code_;
    }

    sqlite3_helper& operator=(sqlite3_helper&& rhs)
    {
        db_ = rhs.db_;
        current_return_code_ = rhs.current_return_code_;
        rhs.db_ = nullptr;
        rhs.current_return_code_ = 0;
    }

    int exec(const char* sql)
    {
        current_return_code_ = sqlite3_exec(db_, sql, nullptr, 0, 0);
        return current_return_code_;
    }

    operator bool() const
    {
        return is_valid(); 
    }

    bool is_valid() const
    {
        return (db_ != nullptr) && (current_return_code_ == SQLITE_OK);
    }

    static bool is_threadsafe()
    {
        return sqlite3_threadsafe();
    }

    int get_last_error() 
    { 
        return current_return_code_; 
    }

    const char* get_last_error_message() 
    {
        return sqlite3_errstr(current_return_code_);
    }

private:
    sqlite3* db_ = nullptr;
    int current_return_code_ = 0;
};
