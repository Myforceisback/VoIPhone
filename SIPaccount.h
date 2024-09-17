#pragma once
#ifndef __SIPACCOUNT_H
#define __SIPACCOUNT_H

#include "pjsua2.hpp"
#include "MyCall.h"
#include <iostream>
#include <gcroot.h>
#include "SIPPresenter.h"
public delegate void UpdateRichTextBoxDelegate(std::string text);
class SIPaccount : public pj::Account
{
public:
	SIPaccount();
	SIPaccount(VoIPhone::SIPPresenter^ pres);
	~SIPaccount();
	//Функция состояния(Register/ Unregister)
	virtual void onRegState(pj::OnRegStateParam& prm);
	//Функция удаления звонка
	void removeCall(pj::Call* call);
	//Функция получения звонка
	virtual void onIncomingCall(pj::OnIncomingCallParam& iprm);
	virtual void onInstantMessage(pj::OnInstantMessageParam& iprm);
	void incall(pj::CallInfo* ci, pj::Call* call, pj::CallOpParam* prm);
public:
	std::string key;
	bool cryptFlag = 0;
 	std::vector<pj::Call*> calls;
	gcroot<UpdateRichTextBoxDelegate^> del;
	gcroot<VoIPhone::SIPPresenter^> presenter;
}; 
#endif // !__SIPACCOUNT_H