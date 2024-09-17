#pragma once
#ifndef __INITPJSUA_H
#define __INITPJSUA_H

#include "pjsua2.hpp"
#include <iostream>

class InitPJSUA : public pj::Endpoint
{
public:
	InitPJSUA() : Endpoint() {};
	virtual pj_status_t onCredAuth(pj::OnCredAuthParam& prm);
};

#endif // !__INITPJSUA_H