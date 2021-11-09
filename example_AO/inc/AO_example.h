#ifndef _AO_EXAMPLE_H_
#define _AO_EXAMPLE_H_

#include "aos.hpp"

enum my_events : Event_driven::Signal
{
    WASTE_EVENT = 3,
    TEST_EVENT,
    LED_BLINK,
    TEST_TRAN,

};

extern Event_driven::Active *const AO_myObj;

#endif //_AO_EXAMPLE_HPP_