#ifndef _AO_EXAMPLE_H_
#define _AO_EXAMPLE_H_

#include "aos.hpp"
#include <iostream>

enum my_events : Event_driven::Signal
{
    WASTE_EVENT = Event_driven::USER_SIG,
    TEST_EVENT,
    LED_BLINK,
    TEST_TRAN,
    TRIG_SUPER,

};

// Active objects' opaque pointers
extern Event_driven::Active *const AO_myObj_1;
extern Event_driven::Active *const AO_myObj_2;

#endif //_AO_EXAMPLE_HPP_