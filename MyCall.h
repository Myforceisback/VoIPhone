#pragma once
#ifndef __MYCALL_H
#define __MYCALL_H

#include "SIPaccount.h"

class MyAudioPlayer : public pj::AudioMediaPlayer
{
public:
    MyAudioPlayer() : AudioMediaPlayer()
    {

    }

    ~MyAudioPlayer()
    {

    }
};

class MyCall : public pj::Call
{
public:
    MyCall(pj::Account& acc, int call_id = PJSUA_INVALID_ID) : pj::Call(acc, call_id) {
        myAcc = (SIPaccount*)&acc;
        
        wav_player = NULL;
    }
    ~MyCall(){
        if (wav_player) {
            delete wav_player;
        }
    }
    virtual void onCallState(pj::OnCallStateParam& prm);//когда состояние вызова изменилось
    virtual void onCallMediaState(pj::OnCallMediaStateParam& prm);//когда состояние микр или динамик изменилось
    bool encr = 0;
private:
    class SIPaccount* myAcc;
    MyAudioPlayer* wav_player;
};
#endif // !__MYCALL_H