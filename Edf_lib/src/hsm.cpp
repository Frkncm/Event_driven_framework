#include "hsm.hpp"
//! helper macro to trigger internal event in an HSM
#define TRIG_STATE(state_, sig_) \
    ((*(state_))(this, sig_))

namespace Event_driven
{

    enum : Signal
    {
        // empty signal for internal use only
        EMPTY_SIG = 0U
    };

    State HSM::top(void *const me, Event const *const e) noexcept
    {
        (void)me;
        (void)e;
        return IGNORED_EVENT;
    }

    void HSM::dispatcher(Event const *const e)
    {
        stateHandlerPtr cState = current_state;

        State rtn; // returned status

        do
        {
            /* Execute the current event */
            rtn = (*cState)(this, e);

            if (rtn == SUPER_STATE)
                cState = target_state;
            
            /* Find the super state by sending empty event */
            Event tempSignal {.sig = EMPTY_SIG};
            rtn = TRIG_STATE(cState, &tempSignal);

        } while (rtn == SUPER_STATE);

        /* If the transition is wanted to be taken */
        if (rtn == HANDLE_TRAN)
        {
            cState = target_state;

            const Event evt{.sig = ENTRY_SIG};

            rtn = (*cState)(this, &evt);
        }
    }

}
