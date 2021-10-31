#include <iostream>
#include "aos.hpp"

using namespace Event_driven;

// Production code
class myObj : public Active
{

public:
    myObj(); // default ctor

    STATE_DEC(Initial_state);
    STATE_DEC(test);
    STATE_DEC(test1);
    STATE_DEC(test2);
};

myObj::myObj() : Active(Initial_state) {}

STATE_DEF(myObj, Initial_state)
{
    std::cout << "ınitial state!\n";
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

static myObj l_myObj;

Active *const AO_myObj = &l_myObj;

int main(void)
{

    static Event const *QueueSto[30];

    AO_myObj->start(QueueSto);

    return esf::run();
}