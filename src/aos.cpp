#include "aos.hpp"

//Active Object Service

namespace Event_driven
{

    void Active::start(Event const **const qSto)
    {
        //std::cout << "AO has been started!\n";
        esf::add(this);
    }

}