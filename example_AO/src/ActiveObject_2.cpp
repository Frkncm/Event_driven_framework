#include "AO_example.h"

// Production code
class myObj_1 : public Event_driven::Active
{

public:
    myObj_1(); // default ctor

    STATE_DEC(Initial_state);
    STATE_DEC(test);
};

static myObj_1 l_myObj;

Event_driven::Active *const AO_myObj_1 = &l_myObj;

myObj_1::myObj_1() : Event_driven::Active(Initial_state) {}

STATE_DEF(myObj_1, Initial_state)
{
    std::cout << "AO2 Initial state!\n";
    auto state = tran(test);
    return state;
}

STATE_DEF(myObj_1, test)
{
    Event_driven::State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        std::cout << "AO2 test is invoked!\n";
        status = STATE_HANDLED;
        break;

    case INIT_SIG:
        std::cout << "AO2 INIT_SIG is invoked!\n";
        status = STATE_HANDLED;
        break;

    case TRIG_SUPER:
        std::cout << "AO2 super event is trigged!\n";
        status = STATE_HANDLED;
        break;

    default:
        status = super(&top);
        break;
    }

    return status;
}

