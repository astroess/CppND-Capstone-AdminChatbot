#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <memory>
#include <wx/wx.h>

class ChatLogic; // forward declaration

/**
 * Middle part of the window containing the dialog between user and chatbot
 */
class ChatBotPanelDialog : public wxScrolledWindow {
    public:
        // constructor / destructor
        ChatBotPanelDialog(wxWindow *parent, wxWindowID id);
        ~ChatBotPanelDialog();

        // getter
        ChatLogic *GetChatLogicHandle() { return _chatLogic.get(); } //Task 1.

        // events
        void paintEvent(wxPaintEvent &evt);
        void paintNow();
        void render(wxDC &dc);

        // proprietary functions
        void AddDialogItem(wxString text, bool isFromUser = true);
        void PrintChatbotResponse(std::string response);
        ChatLogic* GetChatLogic() {return _chatLogic.get();}
    
        DECLARE_EVENT_TABLE()

    private:
        // control elements
        wxBoxSizer *_dialogSizer;
        wxBitmap _image;

        std::unique_ptr<ChatLogic> _chatLogic = std::make_unique<ChatLogic>(); 
};

/**
 * Dialog item shown in ChatBotPanelDialog
 */  
class ChatBotPanelDialogItem : public wxPanel {
    public:
        // constructor / destructor
        ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser);

    private:
        // control elements
        wxStaticBitmap *_chatBotImg;
        wxStaticText *_chatBotTxt;
};

/**
 * Frame containing all control elements
 */ 
class ChatBotFrame : public wxFrame {
    public:
        // constructor / desctructor
        ChatBotFrame(const wxString &title);
        ChatLogic* GetChatLogic() {return _chatLogic;}
        void SetChatLogic(ChatLogic *chatLogic) {_chatLogic = chatLogic;}

    private:
        ChatLogic *_chatLogic;

        static const long ID_STATICTEXT1;
        static const long ID_USERCTRL1;
        static const long ID_ADMINLOGIN;
        static const long ID_PASSWORDTEXT;
        static const long ID_PASSTEXTCTRL;
        static const long ID_ADDANSWERBUTTON;

        // control elements
        ChatBotPanelDialog *_panelDialog;
        wxTextCtrl *_userTextCtrl;
        wxPanel *_topPanel;
        wxGridSizer *_topSizer;
        wxStaticText* _passwordText;
        wxStaticText* _userText;
        wxTextCtrl* _passTextCtrl;
        wxTextCtrl* _textCtrlUser;
        wxButton* _addAnswerButton;
        wxButton* _adminLogin;

        // events
        void OnEnter(wxCommandEvent &WXUNUSED(event));
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnAdminLoginClick(wxCommandEvent& event);
        void OnAddAnswerClick(wxCommandEvent& event);
        void OntopPanelKeyDown(wxKeyEvent& event);
        void AuthenticateAdmin();
};

/**
 * Control panel for background image display
 */ 
class ChatBotFrameBackPanel : public wxPanel {
    public:
        // constructor / desctructor
        ChatBotFrameBackPanel(wxFrame *parent);
};

/**
 * wxWidgets app that hides main()
 */ 
class ChatBotApp : public wxApp {
    public:
        // events
        virtual bool OnInit();
};

#endif /* CHATGUI_H_ */
