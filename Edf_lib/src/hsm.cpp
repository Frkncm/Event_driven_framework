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

    void HSM::init(void const *const e)
    {
        // dispatcher(static_cast<Event const *>(e));
    }

    void HSM::dispatcher(Event const *const e)
    {

        stateHandlerPtr t = current_state;

        stateHandlerPtr cState;

        State rtn; // returned status

        do
        {
            /* Target state is being updated when we are using 
            tran() or super() functions */
            cState = target_state;
            /* Execute the current event */
            rtn = (*cState)(this, e);

        } while (rtn == SUPER_STATE);

        /* If the transition is wanted to be taken */
        if (rtn == HANDLE_TRAN)
        {
            stateHandlerPtr path[MAX_NEST_DEPTH_];

            path[0] = target_state; // save last assigned target state
            path[1] = t;
            path[2] = cState;

            for (; t != cState; t = target_state)
            {
                //Trig the exit state before exiting
                Event tempSignal{.sig = EXIT_SIG};
                if (TRIG_STATE(t, &tempSignal) == STATE_HANDLED)
                {
                    /* Find the super state by sending empty event */
                    // tempSignal{.sig = EMPTY_SIG};
                    // rtn = TRIG_STATE(cState, &tempSignal);
                }
            }

            cState = target_state;

            const Event evt{.sig = ENTRY_SIG};

            rtn = (*cState)(this, &evt);
        }

        //current_state = cState;
    }

}
