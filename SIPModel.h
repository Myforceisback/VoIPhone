#pragma once
#ifndef __SIPMODEL_H
#define __SIPMODEL_H
#include "MyCall.h"
#include "SIPaccount.h"
#include "InitPJSUA.h"
#include "MyBuddy.h"
#include "regform/RegistrationForm.h"
#include "FriendsManager.h"
#include "AddContactForm/AddContactForm.h"
#include <thread>
#include "Magma.h"
namespace VoIPhone {
    ref class SIPPresenter;
    struct uData {
        std::string sipServ;
        std::string sipProxy;
        std::string sipDomain;
        std::string sipUsername;
        std::string sipPassword;
    };

    struct callData {
        pj::Call* call;
        pj::CallOpParam* prm;
    };

    class SIPModel {
    public:
        SIPModel(){}
        //���������� ��� �������� ������
        SIPModel(SIPaccount* acc){
            this->acc = acc;
        }
        ~SIPModel() {

        }
        //�������������� ���������� PJSUA2
        bool initPJ(); //done
        //���������������� ���������� PJSUA2
        bool destroyPJ(); //done
        //��������� ����������� ��������
        bool loginAccount(System::Collections::ArrayList^ destination, SIPPresenter^ pres); //done
        //��������� �������� Buddy
        bool buddyCreate(); //done
        //����� �� ��������
        bool exitAccount(); //done
        //��������� �����(System::String^)
        void makeCallSs(System::String^ destination); //done
        //��������� �����(std::string) ������ �� �����
        void makeCallNs(System::String^ destination);
        void CallThread(std::string callAddress, pj::Call* call, pj::CallOpParam prm); //done
        //���������� ��� ������
        void hangupCalls(); //done
        //���������� ���������
        void sendMessage(System::String^ msg, System::String^ fromUri) const; //done
        //���������� � ���������� ������
        System::String^ addContact(); //done
    private:
        std::string key;
        bool copyFlag = 0;
        bool cryptFlag = 0;
        MyBuddy* buddy = nullptr;
        InitPJSUA core;
       // FriendsManager friends;
        class SIPaccount* acc = nullptr;
        bool _callActive = 0;
    };
}
#endif // !__SIPMODEL_H