#ifndef CHATLOGIC_H_
#define CHATLOGIC_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <atomic>
#include "chatgui.h"
#include "answernode.h"

// forward declarations
class ChatBot;
class GraphEdge;
class GraphNode;

class ChatLogic {
private:
    std::unique_ptr<AnswerNode> _answerNode = std::make_unique<AnswerNode>();
    std::atomic_bool _running = false; // set to stop thread
    std::atomic_bool _closed = false; // set by thread to indicate it ended

    // data handles (not owned)
    ChatBot *_chatBot;
    ChatBotPanelDialog *_panelDialog;
    
    // proprietary type definitions
    typedef std::vector<std::pair<std::string, std::string>> tokenlist;

    // proprietary functions
    template <typename T>
    void AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element);

public:
    // constructor / destructor
    ChatLogic();
    ~ChatLogic();

    // getter / setter
    void SetPanelDialogHandle(ChatBotPanelDialog *panelDialog);
    void SetChatbotHandle(ChatBot *chatbot);

    // proprietary functions
    void LoadAnswerGraphFromFile(std::string filename);  //Old
    void LoadAnswersFromJsonFile(std::string filename);
    void RunDataSynchronization(std::string filename);
    void SendMessageToChatbot(std::string message);
    void SendMessageToUser(std::string message);
    AnswerNode* GetAnswerNode() {return _answerNode.get();}
    wxBitmap *GetImageFromChatbot();
};

#endif /* CHATLOGIC_H_ */