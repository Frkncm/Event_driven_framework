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