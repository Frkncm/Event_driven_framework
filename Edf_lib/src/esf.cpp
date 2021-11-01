// Event-driven framework standard functions

#include "esf.hpp"
#include "aos.hpp"

namespace Event_driven
{
    Active *esf::active_[MAX_ACTIVE + 1U]{nullptr}; // to be used by QF ports only
    std::uint8_t esf::AO_index = 0;

    void esf::initial_events(void)
    {
        Activate_(); // activate AOs to process all events posted so far
    }

    /* Register an active object to be managed by the framework
       This function will add the created active objects into a */
    void esf::add(Active *const a) noexcept
    {
        CRIT_ENTRY_();
        if (AO_index < MAX_ACTIVE)
            active_[AO_index++] = a;
        CRIT_EXIT_();
    }

    int esf::run() noexcept
    {
        // Interrupt needs to be disabled
        INTERRUPT_DISABLE();
        // initial events need to be run
        initial_events();

        //  Run Sartup implementation
        onStartup();
        // interrupt needs to be enabled
        INTERRUPT_ENABLE();

        for (;;)
        {
            onIdle();
        }
        
        return 0;
    }

    void esf::Activate_(void) noexcept
    {

        // std::uint8_t total_AO = AO_index;

        for (int cur_ind = 0; cur_ind < AO_index; cur_ind++)
        {
            active_[cur_ind]->dispatcher(nullptr);
        }
    }

    /* Please override this function when you want to use for 
    production codes */

    void esf::onIdle(void)
    {
        // implement when there is no event for AOs
    }

    void esf::onStartup(void)
    {
        // implement on startup AOs
    }

} //Event_driven namespace