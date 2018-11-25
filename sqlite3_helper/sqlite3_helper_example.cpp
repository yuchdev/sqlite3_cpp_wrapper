#include <iostream>
#include <sqlite3_helper.h>

int main()
{
    int rc = -1;
    if (rc) {
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << "\n";
    }
    else {
        std::cout << "Open database successfully\n\n";
    }
    
    const char* create_table_sql = 
        "create table filetable(hash varchar(16), filename varchar(512), entropy real)";
    rc = sqlite3_exec(db, create_table_sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << sqlite3_errmsg(db) << "\n";
        sqlite3_free(zErrMsg);
    }

    const char* insert1_sql = 
        "insert into filetable(hash, filename, entropy) values ('aaaaaaaaaaaaaaaa', 'C:/Temp/usernames.txt', 1.35)";
    rc = sqlite3_exec(db, insert1_sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << sqlite3_errmsg(db) << "\n";
        sqlite3_free(zErrMsg);
    }

    const char* insert2_sql = 
        "insert into filetable(hash, filename, entropy) values ('BBBBBBBBBBBBBBBB', 'C:/Windows/system32/abc.dll', 6.05)";
    rc = sqlite3_exec(db, insert2_sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << sqlite3_errmsg(db) << "\n";
        sqlite3_free(zErrMsg);
    }

    return 0;
}
