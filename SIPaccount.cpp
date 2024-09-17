#include "SIPaccount.h"
#include "DialogForm.h"
#include "IncomigCallForm.h"
#include <thread>
#include <sstream>
#include "Magma.h"
SIPaccount::SIPaccount()
{
    std::ifstream ifs("D:\\PSU\\DIPLOM\\Debug\\KEY.txt");
    //std::ifstream ifs("C:\\Users\\sdxop\\Desktop\\share\\DIPLOM\\Debug\\KEY.txt");
    if (!ifs)
        std::cerr << "mda";
    ifs >> key;
    if (key != "")
        cryptFlag = 1;
    ifs.close();
}

SIPaccount::SIPaccount(VoIPhone::SIPPresenter^ pres)
{
    std::ifstream ifs("D:\\PSU\\DIPLOM\\Debug\\KEY.txt");
    //std::ifstream ifs("C:\\Users\\sdxop\\Desktop\\share\\DIPLOM\\Debug\\KEY.txt");
    if (!ifs)
        std::cerr << "mda";
    ifs >> key;
    if (key != "")
        cryptFlag = 1;
    ifs.close();
   this->presenter = pres;
}

SIPaccount::~SIPaccount()
{
    std::cout << "*** Account is being deleted: No of calls =" << calls.size() << std::endl;
    for (std::vector<pj::Call*>::iterator it = calls.begin(); it != calls.end(); )
    {
        delete (*it);
        it = calls.erase(it);
    }
}

void SIPaccount::onRegState(pj::OnRegStateParam& prm) {
    pj::AccountInfo ai = getInfo();
    std::cout << (ai.regIsActive ? "*** Register:" : "*** Unregister:")
        << " code=" << prm.code << std::endl;
}

void SIPaccount::removeCall(pj::Call* call)
{
    for (std::vector<pj::Call*>::iterator it = calls.begin();
        it != calls.end(); ++it)
    {
        if (*it == call) {
            calls.erase(it);
            break;
        }
    }
}

void SIPaccount::onIncomingCall(pj::OnIncomingCallParam& iprm)
{
    pj::Call* call = new MyCall(*this, iprm.callId);
    pj::CallInfo ci = call->getInfo();
    pj::CallOpParam prm;

    std::cout << "*** Incoming Call: " << ci.remoteUri << " [" << ci.stateText << "]" << std::endl;
    calls.push_back(call);
    prm.statusCode = PJSIP_SC_OK;
    VoIPhone::IncomigCallForm^ incCall = gcnew VoIPhone::IncomigCallForm(gcnew System::String(ci.remoteUri.c_str()));
    incCall->ShowDialog();
    bool state = incCall->getState();
    if (state) {
        call->answer(prm);
        presenter->answerCall(gcnew System::String(ci.remoteUri.c_str()));
    }
    else {
        calls.pop_back();
        call->hangup(prm);
    }

}

void SIPaccount::incall(pj::CallInfo* ci, pj::Call* call, pj::CallOpParam* prm)
{
    pj_thread_desc desc;
    pj_thread_t* thread = 0;
    pj_thread_register("incomCall", desc, &thread);
    VoIPhone::DialogForm chatForm(*this, ci, call, prm);
    chatForm.ShowDialog();
    if (!this->calls.empty())
        call->hangup(prm);
}

void SIPaccount::onInstantMessage(pj::OnInstantMessageParam& iprm)
{
    if (this->cryptFlag == 1) {
        Magma magma(key.c_str(), "mouse");
        std::string fmsgE = iprm.msgBody;
        std::string fmsgD = "";
        std::istringstream strStreamE(fmsgE);
        std::ostringstream strStreamD(fmsgD);
        magma.decrypt(Magma::Method::CFB, strStreamE, strStreamD);
        presenter->updateChatMsgTextBox(gcnew System::String(iprm.fromUri.c_str()), gcnew System::String((strStreamD.str()).c_str()));
    }
    else
        presenter->updateChatMsgTextBox(gcnew System::String(iprm.fromUri.c_str()), gcnew System::String(iprm.msgBody.c_str()));
}