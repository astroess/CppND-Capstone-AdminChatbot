#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"
#include "answernode.h"

// constructor WITHOUT memory allocation
ChatBot::ChatBot()
{
    // invalidate data handles
    _image = nullptr;
    _chatLogic = nullptr;
    _rootNode = nullptr;
    _answerNode = nullptr;
}

// constructor WITH memory allocation
ChatBot::ChatBot(std::string filename)
{
    
    // invalidate data handles
    _chatLogic = nullptr;
    _rootNode = nullptr;
    _answerNode = nullptr;

    // load image into heap memory
    _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
}

/**
 * Rule 1 (of Rule of 5). Destructor.
 */ 
ChatBot::~ChatBot()
{

    // deallocate heap memory
    if(_image != NULL) // Attention: wxWidgets used NULL and not nullptr
    {
        delete _image;
        _image = NULL;
    }
}

//// STUDENT CODE
//// Task 2. Rule of 5.
/**
 * Rule 2. Copy Constructor
 */ 
ChatBot::ChatBot(const ChatBot &source) {
    _currentNode = source._currentNode;
    _rootNode = source._rootNode;
    _answerNode = source._answerNode;
    _chatLogic = source._chatLogic;
    _image = source._image;

}

/**
 * Rule 3. Copy Assignment Operator 
 */ 
ChatBot &ChatBot::operator=(const ChatBot &source) 
{
    if (this == &source) 
        return *this;
    
    if(_image != NULL) 
        delete _image;

    _image = new wxBitmap(*source._image);
    _currentNode = source._currentNode;
    _rootNode = source._rootNode;
    _answerNode = source._answerNode;
    _chatLogic = source._chatLogic;
    _chatLogic->SetChatbotHandle(this);

    return *this;
}

/**
 * Rule 4. Move Constructor
 */ 
ChatBot::ChatBot(ChatBot &&source) 
{
    _currentNode = source._currentNode;
    _rootNode = source._rootNode;
    _answerNode = source._answerNode;
    _chatLogic = source._chatLogic;
    _image = source._image;

    _chatLogic->SetChatbotHandle(this);

    source._currentNode = nullptr;
    source._rootNode = nullptr;
    source._answerNode = nullptr;
    source._chatLogic = nullptr;    
    delete source._image;
    source._image = NULL;

}

/**
 * Rule 5. Move Assignment Operator
 */ 
ChatBot &ChatBot::operator=(ChatBot &&source)
{
    if (this == &source)
        return *this;

    if(_image != NULL) 
         delete _image;

    _image = new wxBitmap(*source._image);
    _currentNode = source._currentNode;
    _rootNode = source._rootNode;
    _answerNode = source._answerNode;
    _chatLogic = source._chatLogic;
    
    _chatLogic->SetChatbotHandle(this);

    source._currentNode = nullptr;
    source._rootNode = nullptr;
    source._answerNode = nullptr;
    source._chatLogic = nullptr;    
    source._image = NULL;

    return *this;
}

////
//// EOF STUDENT CODE

void ChatBot::ReceiveMessageFromUser(std::string message)
{
    // loop over all edges and keywords and compute Levenshtein distance to query
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists; // format is <ptr,levDist>

    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (auto keyword : edge->GetKeywords())
        {
            EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
            levDists.push_back(ed);
        }
    }

    // select best fitting edge to proceed along
    GraphNode *newNode;
    if (levDists.size() > 0)
    {
        // sort in ascending order of Levenshtein distance (best fit is at the top)
        std::sort(levDists.begin(), levDists.end(), [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
        newNode = levDists.at(0).first->GetChildNode(); // after sorting the best edge is at first position
    }
    else
    {
        // go back to root node
        newNode = _rootNode;
    }

    // tell current node to move chatbot to new node
    _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::ReceiveMessageFromUser2(std::string message) {
    typedef std::pair<std::string, int> AnswerPair;
    std::vector<AnswerPair> answerPairValues;
    
    for (AnswerRec ar : _answerNode->GetAnswerRecs()) {
        for (std::string keyword : ar.keywords) {
            AnswerPair ap{ar.answer, ComputeLevenshteinDistance(keyword, message)};
            answerPairValues.emplace_back(ap);
        }
    }

    std::sort(answerPairValues.begin(), answerPairValues.end(), [](const AnswerPair &a, const AnswerPair &b) { return a.second < b.second; });
    
    //Add a Random element if the there is more than one lowest value.
    int tempVal = answerPairValues.at(0).second;
    int icount;
    for (icount=0; icount < answerPairValues.size(); icount++) {
        AnswerPair tmpPair = answerPairValues.at(icount);
        if (!tempVal == tmpPair.second) {
            break;
        }
    }

    if (icount == 1) {
        _chatLogic->SendMessageToUser(answerPairValues.at(0).first);
    }
    else {
        //Randomize.
    }
    
    answerPairValues.clear();
}

void ChatBot::SetCurrentNode(GraphNode *node)
{
    // update pointer to current node
    _currentNode = node;

    // select a random node answer (if several answers should exist)
    std::vector<std::string> answers = _currentNode->GetAnswers();
    std::mt19937 generator(int(std::time(0)));
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers.at(dis(generator));

    // send selected node answer to user
    _chatLogic->SendMessageToUser(answer);
}

void ChatBot::SetCurrentMessage() {
    
    if (_answerNode->IsDisplayGreeting()) {
        _chatLogic->SendMessageToUser(_answerNode->GREETING_MSG);
        _answerNode->SetDisplayGreeting(false);
    }
    else {

        _chatLogic->SendMessageToUser("");
    }
}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    // convert both strings to upper-case before comparing
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    // compute Levenshtein distance measure between both strings
    const size_t m(s1.size());
    const size_t n(s2.size());

    if (m == 0)
        return n;
    if (n == 0)
        return m;

    size_t *costs = new size_t[n + 1];

    for (size_t k = 0; k <= n; k++)
        costs[k] = k;

    size_t i = 0;
    for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
        {
            size_t upper = costs[j + 1];
            if (*it1 == *it2)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    int result = costs[n];
    delete[] costs;

    return result;
}