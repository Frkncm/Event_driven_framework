#include <iostream>
#include "AO_example.h"

int main(void)
{
    static Event_driven::Event const *QueueSto[30];

    AO_myObj->start(QueueSto);

    return Event_driven::esf::run();
}