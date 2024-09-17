#include "InitPJSUA.h"

pj_status_t InitPJSUA::onCredAuth(pj::OnCredAuthParam& prm)
{
	PJ_UNUSED_ARG(prm);
	std::cout << "*** Callback onCredAuth called ***" << std::endl;
	return PJ_ENOTSUP;
}