#include <iostream>
#include "AO_example.h"

int main(void)
{
    static Event_driven::Event const *QueueSto[30];
    static Event_driven::Event const *QueueSto_1[30];

    AO_myObj->start(QueueSto, 0U);
    AO_myObj_1->start(QueueSto_1, 0U);

    return Event_driven::esf::run();
}

// Overrided onIdle function provided by framework
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