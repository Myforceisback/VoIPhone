#pragma once
#include "pjsua2.hpp"
#include <iostream>
class MyBuddy : public pj::Buddy
{
public:
    MyBuddy() {}
    ~MyBuddy() {}

    virtual void onBuddyState();
};

