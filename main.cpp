#include <iostream>
#include "AO_example.h"

int main(void)
{
    static Event_driven::Event const *QueueSto_1[30];
    static Event_driven::Event const *QueueSto_2[30];

    AO_myObj_1->start(QueueSto_1, 0U);
    AO_myObj_2->start(QueueSto_2, 0U);

    return Event_driven::esf::run();
}

/* onIdle function provided by the framework */
namespace Event_driven
{
    void esf::onIdle(void)
    {
        char ch;
        std::cin >> ch;

        switch (ch)
        {
        case 'l':
            POST(AO_myObj_1, LED_BLINK);
            break;
        case 't':
            POST(AO_myObj_1, TEST_EVENT);
            break;
        case 'e':
            POST(AO_myObj_1, TEST_TRAN);
            break;
        case 's':
            POST(AO_myObj_1, TRIG_SUPER);
            break;
        case 'd':
            POST(AO_myObj_2, TRIG_SUPER);
            break;

        default:
            break;
        }
    }
}