#include <iostream>
#include "ao_service.hpp"

namespace Event_driven
{

    Active *sf::active_[MAX_ACTIVE + 1U]; // to be used by QF ports only
    size_t sf::AO_index = 0;

}