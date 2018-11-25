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

private:
    FileTableReceiver() = default;
    ~FileTableReceiver() = default;

    std::map<std::string, float> rows;
};


static int select_callback(void *raw_data, int column_count, char **column_values, char **column_name) {

    const int finename_column = 0;
    const int entropy_column = 1;
    assert(std::string(column_name[finename_column]) == "filename");
    assert(std::string(column_name[entropy_column]) == "entropy");
    FileTableReceiver::instance().add_row(
        column_values[finename_column], column_values[entropy_column] ? column_values[entropy_column] : "1.0");
    return 0;
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
        std::cout << "Can't open database: " << db.get_last_error_message() << "\n";
    }
    else {
        std::cout << "Open database successfully\n\n";
    }
    db.exec("drop table filetable");
    db.exec("create table filetable(hash varchar(16), filename varchar(512), entropy real)");
    db.exec("insert into filetable(hash, filename, entropy) values ('aaaaaaaaaaaaaaaa', 'C:/Temp/usernames.txt', 1.35)");
    db.exec("insert into filetable(hash, filename, entropy) values ('BBBBBBBBBBBBBBBB', 'C:/Windows/system32/abc.dll', 6.05)");

    db.exec("select filename, entropy from filetable", select_callback);

    const auto& files_fable = FileTableReceiver::instance().get_table();
    for (const auto& item : files_fable) {
        std::cout << item.first << " entropy=" << item.second << '\n';
    }
    // check error handling
    db.close();

    db.exec("insert into filetable(hash, filename, entropy) values ('aaaaaaaaaaaaaaaa', 'C:/Temp/usernames.txt', 1.35)");
    int error_code = db.get_last_error();
    std::string error_msg = db.get_last_error_message();
    std::cout << "Error executing SQL on closed database, code = " << error_code 
        << "; description: " << error_msg << '\n';

    return 0;
}
