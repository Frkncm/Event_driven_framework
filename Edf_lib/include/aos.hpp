#ifndef _AOS_HPP_
#define _AOS_HPP_

#include "hsm.hpp"
#include "esf.hpp"

//Active Object Service

#define CRIT_ENTRY_()
#define CRIT_EXIT_()
#define INTERRUPT_ENABLE()
#define INTERRUPT_DISABLE()

namespace Event_driven
{

    class Active : public HSM
    {

    public:
        Active() = delete;

        Active(stateHandlerPtr initial_state) : HSM(initial_state) {}

        void start(Event const **const qSto, void const * const par);

        bool post_(Event const *const e) noexcept;

    private:
        friend class sf;
    };

} // Event_driven namespace

#endif //_AOS_HPP_