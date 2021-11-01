#include "AO_example.h"
#include <iostream>

// Production code
class myObj : public Event_driven::Active
{

public:
    myObj(); // default ctor

    STATE_DEC(Initial_state);
    STATE_DEC(test);
    STATE_DEC(test1);
    STATE_DEC(test2);
};

static myObj l_myObj;

Event_driven::Active *const AO_myObj = &l_myObj;

myObj::myObj() : Event_driven::Active(Initial_state) {}

STATE_DEF(myObj, Initial_state)
{
    std::cout << "Initial state!\n";
    auto state = tran(test);
    return state;
}

STATE_DEF(myObj, test)
{
    std::cout << "test is invoked!\n";
    auto state = tran(test1);
    return state;
}

STATE_DEF(myObj, test1)
{
    std::cout << "test1 is invoked!\n";
    auto state = tran(test2);
    return state;
}

STATE_DEF(myObj, test2)
{
    std::cout << "test2 is invoked!\n";
    return STATE_HANDLED;
}

