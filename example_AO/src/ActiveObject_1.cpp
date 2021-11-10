#include "AO_example.h"

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
    std::cout << "AO1 Initial state!\n";
    auto state = tran(test);
    return state;
}

STATE_DEF(myObj, test)
{
    Event_driven::State status;
    switch (e->sig)
    {
    case ENTRY_SIG:
        std::cout << "AO1 test is invoked!\n";
        status = STATE_HANDLED;
        break;

    case INIT_SIG:
        status = tran(test1);
        break;

    case TRIG_SUPER:
        std::cout << "AO1 super event is trigged!\n";
        status = STATE_HANDLED;
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
        std::cout << "AO1 test1 is invoked!\n";
        status = STATE_HANDLED;
        break;

    case INIT_SIG:
        status = tran(test2);
        break;

    case LED_BLINK:
        std::cout << "AO1 led blink event is trigged!\n";
        status = STATE_HANDLED;
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
        std::cout << "AO1 test2 is invoked!\n";
        status = STATE_HANDLED;
        break;

    case INIT_SIG:
        POST(AO_myObj, TEST_EVENT);
        status = STATE_HANDLED;
        break;

    case TEST_TRAN:
        status = tran(test1);
        break;

    case TEST_EVENT:
        std::cout << "AO1 test event is trigged!\n";
        status = STATE_HANDLED;
        break;

    default:
        status = super(&test1);
        break;
    }

    return status; //STATE_HANDLED;
}

// onIdle function provided by framework
namespace Event_driven
{
    void esf::onIdle(void)
    {
        // implement when there is no event for AOs
        char ch;
        Event evt;
        std::cin >> ch;

        switch (ch)
        {
        case 'l':
            POST(AO_myObj, LED_BLINK);
            break;
        case 't':
            POST(AO_myObj, TEST_EVENT);
            break;
        case 'e':
            POST(AO_myObj, TEST_TRAN);
            break;
        case 's':
            POST(AO_myObj, TRIG_SUPER);
            break;
        case 'd':
            POST(AO_myObj_1, TRIG_SUPER);
            break;

        default:
            break;
        }
    }
}