#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream> 
#include <memory>

using std::cout;
using std::cin;
using std::string;

bool processAddAdminUser(std::vector<string> *pdata);
void processListAdminUser();
void processDeleteAdminUser();
bool loadAdminFile(std::ifstream *adminFile, std::vector<string> *admindata);
void createLockFile();
bool isLockFilePresent();
std::string encrypt(std::string msg);
std::string decrypt(std::string msg);

const std::string userAdminFilePathLock{"./data/adminusers.lock"};
const std::string userAdminFile{"./data/adminusers.txt"};

int main() {
    bool continueParentLoop;
    std::string pVal;
    std::ifstream adminFile;
    std::unique_ptr<std::vector<string>> admindata = std::make_unique<std::vector<string>>();

    cout << "***Welcome to the Admin User Configuration Tool for the AdminChatbot***\n\n";    
    if (isLockFilePresent()) {
        _Exit(-1);
    }

    do {
        continueParentLoop = false;
        cout << "Please select one of the following numbers 3 numbers (type actual number), \n or Q to quit.\n";
        cout << "1. Add Admin User\n";
        cout << "2. Delete Admin User\n";
        cout << "3. List Current Admin Users\n";
        cout << "Q. Quit\n";
        cout << "$ ";

        cin >> pVal;
        cout << "\n";
        if (pVal.compare("Q") == 0 || pVal.compare("q") == 0) {
            break;
        }

        if (cin.fail() || (pVal.compare("1") != 0 && pVal.compare("2") != 0) &&
                           pVal.compare("3") != 0) {
            continueParentLoop = true;
        }
        else {
            //Create lock file
            createLockFile();

            //Then load file.
            if (!loadAdminFile(&adminFile, admindata.get())) {
                cout << "There was a problem opening the adminUsers file.\n";
            
            }
        }
        
        if (pVal.compare("1") == 0) {
            continueParentLoop = processAddAdminUser(admindata.get());
            for (string itr : *admindata) {
                cout << itr << "\n";
            }

        }
        else if (pVal.compare("2") == 0) {
            processDeleteAdminUser();
        }
        else if (pVal.compare("3") == 0) {
            processListAdminUser();
        }
        
        //Resave the new list.

        cin.clear();
    } while (continueParentLoop);

    cout << "***Thanks. The adminusers utility program has ended.***\n\n";

    //Remove lock file.
    remove(userAdminFilePathLock.c_str());

    return 0;
}

//Implementation
bool processAddAdminUser(std::vector<string> *pdata) {
    string uname, pass;

    cout << "User Name: ";
    cin >> uname;
    cout << "Password (between 8 and 12 chars): ";
    cin >> pass;

    pdata->emplace_back(uname + "=" + pass);
    cout << "\nAdmin User has been added.\n\n";

    return true;
}

void processListAdminUser() {
    cout << "Got to processListAdminUser()\n";
}

void processDeleteAdminUser() {
    cout << "Got to processDeleteAdminUser()\n";

}

void createLockFile() {
    std::fstream fs;
    fs.open(userAdminFilePathLock, std::ios::out);
    fs.close();
}

bool isLockFilePresent() {
    
    if (FILE *file = fopen(userAdminFilePathLock.c_str(), "r")) {
        cout << "Sorry, lock file is still present.  This means that someone else is running this program, \n";
        cout << "Or the file was left there due to a previous crash. To continue, please remove it if you know that\n";
        cout << "you're the only person using the adminusers utility.\n\n";
        cout << "Path of lock file is: " + userAdminFilePathLock + "\n\n";
        fclose(file);

        return true;
    }
    else {
        return false;
    }
}

bool loadAdminFile(std::ifstream *adminFile, std::vector<string> *pdata) {
    adminFile->open(userAdminFile, std::fstream::in | std::fstream::out | std::fstream::app);
    if (adminFile->is_open()) {
        for (std::string line; std::getline(*adminFile, line); ) {
            pdata->emplace_back(line);
        }

        return true;
    }
    else {
        std::cerr << "adminusers file could not be opened." << "\n";
        return false;
    }
       
}

/**
 * XOR encryption
 */ 
std::string encrypt(std::string msg) {
    string key = "S1Troessen2341&#reuther";

    // Make sure the key is at least as long as the message
    std::string tmp(key);
    while (key.size() < msg.size())
        key += tmp;
    
    // And now for the encryption part
    for (std::string::size_type i = 0; i < msg.size(); ++i)
        msg[i] ^= key[i];
    return msg;
}

std::string decrypt(std::string msg) {
    return encrypt(msg); // lol
}


// void eraseFileLine(std::string path, std::string eraseLine) {
//     std::string line;
//     std::ifstream fin;
    
//     fin.open(path);
//     // contents of path must be copied to a temp file then
//     // renamed back to the path file
//     std::ofstream temp;
//     temp.open("temp.txt");

//     while (getline(fin, line)) {
//         // write all lines to temp other than the line marked for erasing
//         if (line != eraseLine)
//             temp << line << std::endl;
//     }

//     temp.close();
//     fin.close();

//     // required conversion for remove and rename functions
//     const char * p = path.c_str();
//     remove(p);
//     rename("temp.txt", p);
// }

// void exampleJson() {

//     // 1. Parse a JSON string into DOM.
//     const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
//     Document d;
//     d.Parse(json);
 
//     // 2. Modify it by DOM.
//     Value& s = d["stars"];
//     s.SetInt(s.GetInt() + 1);
 
//     // 3. Stringify the DOM
//     StringBuffer buffer;
//     Writer<StringBuffer> writer(buffer);
//     d.Accept(writer);
 
//     // Output {"project":"rapidjson","stars":11}
//     std::cout << buffer.GetString() << std::endl;

// }