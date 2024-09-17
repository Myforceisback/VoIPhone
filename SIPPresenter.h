#pragma once
#ifndef __SIPPRESENTER_H
#define __SIPPRESENTER_H
//#include "SIPModel.h"
namespace VoIPhone {
    class SIPModel;
    ref class MainForm;
    ref class SIPPresenter{
    public:
        SIPPresenter(MainForm^ v);
        void OnLoginRegButtonClicked();
        void OnExitButtonClicked();
        void OnAddContactButtonClicked();
        void destroyLib();
        void OnUpdateSelectedContact();
        void OnMakeCallButtonClicked();
        void OnSendMessageButtonClicked();
        void OnSendFileButtonClicked();
        void updateChatMsgTextBox(System::String^ url, System::String^ msg);
        void updateChat(array<Object^>^ args);
        void UpdateChatHelper();
        array<Object^>^ GetArgs();
        void answerCall(System::String^ uri);
        void updateCall();
    private:
        array<Object^>^ incMsg;
        array<Object^>^ incCall;
        SIPModel* model;
        MainForm^ view;
        bool _destroy = 0;
    };
}
#endif // !__SIPPRESENTER_H

