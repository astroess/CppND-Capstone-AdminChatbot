/**
 * This program is a command line utility application to add and/or delete admin users
 * from the adminusers.txt data file.  The file is used to authenticate admin users 
 * in the AdminChatBot application.
 */ 
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream> 
#include <memory>
#include <algorithm>
#include <signal.h>

using std::cout;
using std::cin;
using std::string;

//Comments for these functions are written before the implementation code.
void processAddAdminUser(std::vector<string> *pdata);
void processListAdminUser(std::vector<string> *pdata);
void processDeleteAdminUser(std::vector<string> *pdata);
bool loadAdminFile(std::vector<string> *admindata);
void createLockFile();
bool isLockFilePresent();
bool writeOutAdminFile(std::vector<string> *admindata);
bool isUserPassValid(string &user, string &pass, std::vector<string> *pdata);
bool isOnlyASCIIAndNotSpace(const std::string &strval);
std::string encrypt(std::string msg);
std::string decrypt(std::string msg);

const std::string userAdminFilePathLock{"../data/adminusers.lock"};
const std::string userAdminFile{"../data/adminusers.txt"};

//For debugging purposes
//const std::string userAdminFilePathLock{"./data/adminusers.lock"};
//const std::string userAdminFile{"./data/adminusers.txt"};

/**
 * Callback function for the Signal Ctrl-C interrupt.  Users will not be able to 
 * Ctrl-C out of the application.
 */ 
void signal_callback_handler(int signum) {
    cout << "Please go to main menu and type \"Q\" to exit program." << "\n";
}
/**
 * Entry point for this utility application.  While this program is running, a 
 * lock file is created so that the data file doesn't get written to concurrently. 
 */ 
int main() {
    signal(SIGINT, signal_callback_handler);
    bool continueParentLoop = true, fileLoaded = false;
    std::string pVal;
    std::unique_ptr<std::vector<string>> admindata = std::make_unique<std::vector<string>>();

    cout << "***Welcome to the Admin User Configuration Tool for the AdminChatbot***\n\n";    
    if (isLockFilePresent()) {
        _Exit(-1);
    }

    do {
        
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

        if (!(cin.fail() || (pVal.compare("1") != 0 && pVal.compare("2") != 0) &&
                           pVal.compare("3") != 0)) {
            //Create lock file
            createLockFile();

            //Then load file.
            if (!fileLoaded) {
                if (!loadAdminFile(admindata.get())) {
                    cout << "There was a problem opening the adminUsers file.\n";    
                    cout << "Please make sure to run this program from the \"build\" directory\n";
                    break;  
                }
            }
            fileLoaded = true;
        }
        
        if (pVal.compare("1") == 0) {
            processAddAdminUser(admindata.get());
        }
        else if (pVal.compare("2") == 0) {
            processDeleteAdminUser(admindata.get());
        }
        else if (pVal.compare("3") == 0) {
            processListAdminUser(admindata.get());
        }
        
        cin.clear();
    } while (continueParentLoop);

    //Write out adminusers file.
    if (!writeOutAdminFile(admindata.get())) {
        std::cerr << "There was a problem writing the admin user's data to the file.\n";
    }
    else {
        cout << "***Thanks. The adminusers utility program has ended.***\n\n";
    }

    //Remove lock file.
    remove(userAdminFilePathLock.c_str());

    return 0;
}

/**
 * Function to add an admin user.  The method will call a encryption routine to 
 * encrypt the password before adding it to the file.
 */ 
void processAddAdminUser(std::vector<string> *pdata) {
    string uname, pass, qvar;
    string navigate = "";
    bool continueAddLoop;
    cout << "Remember your credentials for later use.\n";

    do {
        continueAddLoop = false;
        cout << "User Name (between 8 and 12 ASCII characters, no spaces): ";
        getline(cin >> std::ws, uname);

        cout << "Password (between 8 and 12 ASCII characters, no spaces): ";
        getline(cin >> std::ws, pass);

        if (!isUserPassValid(uname, pass, pdata)) {
            cout << "Either the user name or password did not pass validation.\n";
            cout << "Please enter correct number to continue.\n";
            cout << "0. Back to main menu.\n";
            cout << "1. Try adding credentials again.\n";
            cin >> navigate;
            if (!navigate.compare("0") == 0) {
                navigate = "";
                continueAddLoop = true;
            }
        }
        
        cin.clear();

    } while(continueAddLoop);

    if (navigate.length() == 0) {
        pdata->emplace_back(uname + "::::" + encrypt(pass));
        cout << "\n***Admin User has been added.***\n\n";
    }

}

/**
 * Function to list the current users in memory.  Once application exit, the
 * list will be written out to the data file. 
 */ 
void processListAdminUser(std::vector<string> *pdata) {
    cout << "Here are the current list of Users.\n";
    cout << "The passwords are encrypted.\n";
    cout << "The user names and passwords are delimited by \"::::\"\n";
    cout << "----------------------------------------------------\n";
    for (string itr : *pdata) {
        cout << itr << "\n";
    }
    cout << "----------------------------------------------------\n";
    cout << "\n";
}

/**
 * Function to delete a user from the current vector list in memory.
 */ 
void processDeleteAdminUser(std::vector<string> *pdata) {
    cout << "***Delete one value below.***\n";
    int recCount = 1, recSel;
    for (string itr : *pdata) {
        cout << std::to_string(recCount) << ". " << itr << std::endl;
        recCount++;
    }

    //Please select the number to delete.
    do {
        cout << "\nPlease select the number of the record to delete. \n";
        cout << "Or select 0 to go back to the main menu.\n";
        cin >> recSel;
        if (!cin.fail()) {
            if (recSel == 0) { break; }
            if (recSel >= recCount || recSel < 1) {
                cout << "There is no record with that number, Please select a real record number.\n";
            }
            else {
                //Do actual remove from vector.
                pdata->erase(pdata->begin() + (recSel - 1));
                cout << "\n***Record " << recSel << " was successfully deleted.***\n";
                break;
            }
        }

        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    } while (true);
    cout << "\n";
}

/**
 * Function to determine if the chosen username and password are valid.  If valid, the function
 * returns true. 
 */ 
bool isUserPassValid(string &user, string &pass, std::vector<string> *pdata) {
    //Check length.
    if (user.length() > 12 || user.length() < 8) {
        return false;
    }

    if (pass.length() > 12 || pass.length() < 8) {
        return false;
    }

    if (!isOnlyASCIIAndNotSpace(user) || !isOnlyASCIIAndNotSpace(pass)) {
        return false;
    }

    //Check for duplicates
    for (string itr : *pdata) {
        std::string delimiter = "::::";
        std::string ucheck = itr.substr(0, itr.find(delimiter)); 

        if (user.compare(ucheck) == 0) {
            return false;
        }
    }

    return true;
}

/**
 * Function to create the lock file called adminusers.lock.
 */ 
void createLockFile() {
    std::fstream fs;
    fs.open(userAdminFilePathLock, std::ios::out);
    fs.close();
}

/**
 * This function is part of the username and password validation.  Only
 * ASCII characters are allowed with the exception of a space.
 */ 
bool isOnlyASCIIAndNotSpace(const std::string &strval) {
    for (auto c: strval) {
        if (static_cast<unsigned char>(c) > 127 || static_cast<unsigned char>(c) == 32) {
            return false;
        }
    }

    return true;
}

/**
 * Function to determin if the lock file is present.  If it is, then application
 * will exit.
 */ 
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

/**
 * Function to load the adminusers.txt data file into the primary data vector.
 */ 
bool loadAdminFile(std::vector<string> *pdata) {
    std::ifstream adminFile(userAdminFile, std::fstream::in);
    if (adminFile.is_open()) {
        for (std::string line; std::getline(adminFile, line); ) {
            pdata->emplace_back(line);
        }
        
        adminFile.close();
        return true;
    }
    else {
        return false;
    }
    
}

/**
 * This function ecrypts a string using XOR encryption.
 */ 
std::string encrypt(std::string msg) {
    string key = "Stroessenreuther";

    // Make sure the key is at least as long as the message
    std::string tmp(key);
    while (key.size() < msg.size()) {
        key += tmp;
    }

    // And now for the encryption part
    for (std::string::size_type i = 0; i < msg.size(); ++i) {
        msg[i] ^= key[i];
    }

    return msg;
}

/**
 * Decrypts a string that was encrypted with the function above.
 */ 
std::string decrypt(std::string msg) {
    return encrypt(msg); 
}

/**
 * This function writes out the changed data from the vector to the 
 * output data file.  The file is then used to authenticate admin users for the
 * AdminChatbot.
 */ 
bool writeOutAdminFile(std::vector<string> *admindata) {
    std::fstream outData(userAdminFile, std::ios::out | std::ios::trunc);
    
    if (outData) {
        for (string itr : *admindata) {
            outData << itr;
            outData << "\n";
        }

        outData.close();
        return true;
    }
    else {
        std::cerr << "adminusers file could not be opened." << "\n";
        return false;
    }
}
