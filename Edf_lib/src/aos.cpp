#include "aos.hpp"

//Active Object Service

namespace Event_driven
{

    void Active::start(Event const **const qSto, void const * const par)
    {
        //std::cout << "AO has been started!\n";
        esf::add(this);
        this->init();
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