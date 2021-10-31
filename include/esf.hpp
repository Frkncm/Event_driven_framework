#ifndef _ESF_HPP_
#define _ESF_HPP_

#include "aos.hpp"
// Event-driven framework standard functions

#define MAX_ACTIVE 32U

namespace Event_driven
{
    // Forward decleration
    class Active;

    class esf
    {

    public:
        esf() {}

        static void initial_events(void);

        /* Register an active object to be managed by the framework
       This function will add the created active objects into a */
        static void add(Active *const a) noexcept;

        static int run() noexcept;

        static void Activate_(void) noexcept;

        /* The implementation that needs to be executed during in
            idle times should be added under this function */
        static void onIdle(void);

        static void onStartup(void);

    private:
        static Active *active_[MAX_ACTIVE + 1U]; // to be used by QF ports only
        static std::uint8_t AO_index;
    };
}

#endif // _ESF_HPP_