#include "sqlite3_helper.h"
#include <iostream>
#include <string>

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
    
    db.exec("create table filetable(hash varchar(16), filename varchar(512), entropy real)");
    db.exec("insert into filetable(hash, filename, entropy) values ('aaaaaaaaaaaaaaaa', 'C:/Temp/usernames.txt', 1.35)");
    db.exec("insert into filetable(hash, filename, entropy) values ('BBBBBBBBBBBBBBBB', 'C:/Windows/system32/abc.dll', 6.05)");

    // check error handling
    db.close();

    db.exec("insert into filetable(hash, filename, entropy) values ('aaaaaaaaaaaaaaaa', 'C:/Temp/usernames.txt', 1.35)");
    int error_code = db.get_last_error();
    std::string error_msg = db.get_last_error_message();
    std::cout << "Error executing SQL on closed database, code = " << error_code 
        << "; description: " << error_msg << '\n';

    return 0;
}
