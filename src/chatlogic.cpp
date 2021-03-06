#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include <algorithm>
#include <thread>

#include "chatbot.h"
#include "chatlogic.h"
#include "adminutility.h"

ChatLogic::ChatLogic() { }

/**
 * The Destructor will stop the external file sychronizing thread that was started
 * when the application stated.
 */
ChatLogic::~ChatLogic() {
    _running = false; // stop external thread

    while(!_closed) // you could code a timeout here 
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

/**
 * The LoadAnswersFromJsonFile function loads the json data from the 
 * answers.json file located in the data directory.  It also sets the AnswerNode
 * data for the running application.
 */
void ChatLogic::LoadAnswersFromJsonFile(std::string filename) {
    AdminUtility au;
    std::vector<AnswerRec> answerRecs = au.GetAnswerRecsFromFile(filename);
    if (answerRecs.size() == 0) {
        std::cerr << "There was a problem retrieving the Json data.\n";
        exit(1);
    }

    _answerNode->SetAnswerRecs(answerRecs);
    _answerNode->SetRawJsonData(*au.GetJsonFromAnswerRecs());
    _answerNode->SetDisplayGreeting(true);
    
    if (!_answerNode->CheckAndUpdateFileJson(filename)) {
        std::cerr << "There was a problem locating the Json data file.\n";
        exit(1);
    }

    //Start external thread to monitor file changes.
    std::thread externalThread(&ChatLogic::RunDataSynchronization, this, filename);
    externalThread.detach();
    _running = false;

    ChatBot stackChatBot = ChatBot("../images/chatbot.png");   
    stackChatBot.SetChatLogicHandle(this);   
    stackChatBot.SetAnswerNode(_answerNode.get());
    _answerNode->MoveChatbotHere(std::move(stackChatBot));    
}

/**
 * The RunDataSynchronization function runs in a separate thread checking to see
 * if the filename (answers.json) file changed.  If there is a change the data
 * will re-load into the AnswerNode instance.  This allows the data to stay in-sync
 * during runtime even if the answers.json is changed from an outside application.
 */ 
void ChatLogic::RunDataSynchronization(std::string filename) {
    AdminUtility au;
    _running = true;
    do {
        struct stat st;   
        int ierr = stat (filename.c_str(), &st);

        if (ierr == 0) {
            if (_answerNode->GetFileDate() != st.st_mtime) {
                std::vector<AnswerRec> answerRecs = au.GetAnswerRecsFromFile(filename);
                _answerNode->SetAnswerRecs(answerRecs);
                _answerNode->SetRawJsonData(*au.GetJsonFromAnswerRecs());
                _answerNode->SetFileDate(st.st_mtime);
            }
        }

       std::this_thread::sleep_for(std::chrono::milliseconds(2000)); 
    }
    while (_running);

    _closed = true;
}


void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog) {
    _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot) {
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(std::string message) {
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message) {
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot() {
    return _chatBot->GetImageHandle();
}
