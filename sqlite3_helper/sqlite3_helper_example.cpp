#include "sqlite3_helper.h"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <map>

class FileTableReceiver
{
public:
    
    static FileTableReceiver& instance()
    {
        static FileTableReceiver receiver;
        return receiver;
    }

    void add_row(const char* filename, const char* entropy)
    {
        try{
            rows[std::string(filename)] = std::stof(std::string(entropy));
        }
        catch (const std::invalid_argument& e){
            rows[std::string(filename)] = 1.0;
        }
    }

    const std::map<std::string, float>& get_table()
    {
        return rows;
    }

    static int select_callback(void *raw_data, int column_count, char **column_values, char **column_name);

private:
    FileTableReceiver() = default;
    ~FileTableReceiver() = default;

    std::map<std::string, float> rows;
};

// static 
int FileTableReceiver::select_callback(void *raw_data, int column_count, char **column_values, char **column_name)
{

    const int finename_column = 0;
    const int entropy_column = 1;
    assert(std::string(column_name[finename_column]) == "filename");
    assert(std::string(column_name[entropy_column]) == "entropy");
    FileTableReceiver::instance().add_row(
        column_values[finename_column], column_values[entropy_column] ? column_values[entropy_column] : "1.0");
    return 0;
}


void check_errors(const sqlite3_helper& db)
{
    if (db.get_last_error() != SQLITE_OK) {
        std::cout << "Error executing SQL on closed database, code = " << db.get_last_error()
            << "; description: " << db.get_last_error_message() << '\n';
    }
}


void select_results()
{
    const auto& files_fable = FileTableReceiver::instance().get_table();
    for (const auto& item : files_fable) {
        std::cout << '|' << item.first << '|' << item.second << '|' << '\n';
    }

}

int main()
{
    if (sqlite3_helper::is_threadsafe()) {
        std::cout << "Sqlite3 build is threadsafe\n";
    }
    else {
        std::cout << "Sqlite3 build is not threadsafe!\n";
    }

    sqlite3_helper db("files.db");
    if (!db) {
        check_errors(db);
    }
    else {
        std::cout << "Open database successfully\n\n";
    }
    db.exec("drop table filetable");
    check_errors(db);

    std::cout << "Perform CREATE TABLE\n";

    // check CREATE TABLE query
    db.exec("CREATE TABLE filetable(id INTEGER PRIMARY KEY AUTOINCREMENT, filename TEXT, entropy REAL)");
    check_errors(db);

    std::cout << "Perform INSERT INTO\n";

    // check INSERT INTO query
    db.exec("INSERT INTO filetable(filename, entropy) VALUES ('C:/Temp/usernames.txt', 1.35)");
    check_errors(db);

    db.exec("INSERT INTO filetable(filename, entropy) VALUES ('C:/Windows/system32/abc.dll', 6.05)");
    check_errors(db);

    std::cout << "Perform SELECT\n";

    // check SELECT query
    db.exec("SELECT filename, entropy FROM filetable", &FileTableReceiver::select_callback);
    check_errors(db);
    select_results();
    
    std::cout << "Perform UPDATE\n";

    // check UPDATE query
    db.exec("UPDATE filetable SET entropy = 7.23 WHERE id=1");
    check_errors(db);

    std::cout << "Perform SELECT after UPDATE\n";

    // check SELECT query
    db.exec("SELECT filename, entropy FROM filetable", &FileTableReceiver::select_callback);
    check_errors(db);
    select_results();

    std::cout << "Check error handling on closed database\n";

    // check error handling
    db.close();

    db.exec("INSERT INTO filetable(filename, entropy) VALUES ('C:/Temp/usernames.txt', 1.35)");
    check_errors(db);

    return 0;
}
