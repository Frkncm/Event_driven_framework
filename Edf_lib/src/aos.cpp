#include "aos.hpp"

//Active Object Service

namespace Event_driven
{

    void Active::start(Event const **const qSto)
    {
        //std::cout << "AO has been started!\n";
        esf::add(this);
    }

    bool Active::post_(Event const *const e) noexcept
    {
        bool status;

        CRIT_ENTRY_();

        dispatcher(e);

        CRIT_EXIT_();

        return true;
    }

}