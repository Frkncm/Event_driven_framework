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
    Event_driven::State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        std::cout << "test is invoked!\n";
        status = STATE_HANDLED;
        break;

    case INIT_SIG:
        status = tran(test1);
        break;

    default:
        status = super(&top);
        break;
    }

    return status;
}

STATE_DEF(myObj, test1)
{
    Event_driven::State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        std::cout << "test1 is invoked!\n";
        status = STATE_HANDLED;
        break;

    case INIT_SIG:
        status = tran(test2);
        break;

    default:
        status = super(&test);
        break;
    }
    return status;
}

STATE_DEF(myObj, test2)
{
    Event_driven::State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        std::cout << "test2 is invoked!\n";
        status = STATE_HANDLED;
        break;

    case INIT_SIG:
        Event_driven::Event evt;
        evt.sig = TEST_EVENT;
        AO_myObj->post_(&evt);
        status = STATE_HANDLED;
        break;

    case TEST_EVENT:
        std::cout << "test event is trigged!\n";
        status = STATE_HANDLED;
        break;

    default:
        status = super(&test1);
        break;
    }

    return status; //STATE_HANDLED;
}
