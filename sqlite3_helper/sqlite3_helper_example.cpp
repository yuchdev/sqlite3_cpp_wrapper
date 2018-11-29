#include "sqlite3_helper.h"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <codecvt>
#include <sstream>


struct FileInfo
{
    std::wstring path;
    double entropy;
};

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
        std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;
        std::wstring wfilename = cv.from_bytes(std::string(filename));

        try{
            rows[wfilename] = std::stof(std::string(entropy));
        }
        catch (const std::invalid_argument&){
            rows[wfilename] = -1.0;
        }
    }

    const std::map<std::wstring, float>& get_table()
    {
        return rows;
    }

    static int select_callback(void *raw_data, int column_count, char **column_values, char **column_name);

private:
    FileTableReceiver() = default;
    ~FileTableReceiver() = default;

    std::map<std::wstring, float> rows;
};

// static 
int FileTableReceiver::select_callback(void *raw_data, int column_count, char **column_values, char **column_name)
{
    const int finename_column = 0;
    const int entropy_column = 1;
    assert(std::string(column_name[finename_column]) == "filename");
    assert(std::string(column_name[entropy_column]) == "entropy");
    std::string path(column_values[finename_column]);
    std::string entropy(column_values[entropy_column]);
    FileTableReceiver::instance().add_row(path.c_str(), entropy.c_str());
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
        std::wcout << L'|' << item.first << L'|' << item.second << L'|' << std::endl;
    }

}

void unicode_test()
{
    sqlite3_helper db("unicode_files.db");
    if (!db) {
        check_errors(db);
    }
    else {
        std::cout << "Open database successfully\n\n";
    }

    std::cout << "Set UTF-8 as charset\n";
    db.exec(R"(PRAGMA encoding = "UTF-8")");
    check_errors(db);

    db.exec("DROP TABLE IF EXISTS files");
    check_errors(db);

    std::cout << "Perform CREATE TABLE\n";
    db.exec("CREATE TABLE files(id INTEGER PRIMARY KEY AUTOINCREMENT, filename TEXT, entropy REAL)");
    check_errors(db);

    std::vector<FileInfo> paths{
        { FileInfo{{ L"'Im just a file.txt'" }, 4.01}},
        { FileInfo{{ L"'Bröther may I have some lööps.docx'" }, 4.98}},
        { FileInfo{{ L"'Бутылка для рашкована (Чечня круто).mp4'" }, 5.94}},
        { FileInfo{{ L"'אלברט איינשטיין.pdf'" }, 7.98}},
        { FileInfo{{ L"'西伯利亚是中国人.map'" }, 2.22}}
    };

    std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;
    
    for (const FileInfo& file_info : paths) {
        std::wstringstream wss;
        wss << L"INSERT INTO files(filename, entropy) VALUES ("
            << file_info.path << L", "
            << file_info.entropy << L")";
        std::wstring wsql(wss.str());
        std::string sql = cv.to_bytes(wsql);
        db.exec(sql.c_str());
        check_errors(db);
    }

    db.exec("SELECT filename, entropy FROM files", &FileTableReceiver::select_callback);
    check_errors(db);
    // Do not perform select_results() because of multiple locales
}




void simple_test()
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
}

int main()
{
    // Perform simple database creation test and error handling
    simple_test();

    // Perform test with Unicode data convertings into UTF-8
    unicode_test();

    return 0;
}
