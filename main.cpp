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
};

myObj::myObj() : Active(Initial_state) {}

STATE_DEF(myObj, Initial_state)
{   
    std::cout << "I am doing something for the initial state!\n";
    auto state = tran(test);
    return state;
}

STATE_DEF(myObj, test)
{
    std::cout << "test is invoked!\n";
    auto state = tran(test1);
    return STATE_HANDLED;
}

STATE_DEF(myObj, test1)
{
    std::cout << "test1 is invoked!\n";
    auto state = tran(test);
    return STATE_HANDLED;
}

static myObj l_myObj;

Active *const AO_myObj = &l_myObj;

int main(void)
{

    static Event const *QueueSto[30];

    std::cout << &AO_myObj << std::endl;

    AO_myObj->start(QueueSto);

    // Event myevt;

    // l_myObj.dispatcher(nullptr);
    //AO_myObj->dispatcher(&myevt);
    //AO_myObj->dispatcher(&myevt);
    //AO_myObj->dispatcher(&myevt);

    esf::run();

    std::cout << "working!\n";
    return 0;
}