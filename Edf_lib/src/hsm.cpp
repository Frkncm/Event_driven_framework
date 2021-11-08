#include "hsm.hpp"
#include <assert.h>
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

    void HSM::init(void)
    {
        stateHandlerPtr t = current_state;

        stateHandlerPtr cState;

        Event tempSignal;

        // Execute the initial function at first with empty event
        State rtn = (*target_state)(this, static_cast<Event const *>(0U));

        assert(rtn == HANDLE_TRAN);

        do
        {
            stateHandlerPtr hierarchy[MAX_NEST_DEPTH]; // Hierarchy holder
            std::int_fast8_t indx = 0;

            // Save updated target state (the update is occuring during transition)
            hierarchy[0] = target_state;

            // Trig the current state with empty signal
            tempSignal.sig = EMPTY_SIG;
            (void)TRIG_STATE(target_state, &tempSignal);

            while (target_state != t)
            {
                /* Check if target is top() function. When we hit the top
                function, we are in superstate. */
                ++indx;
                /* Save superstate of each states */
                hierarchy[indx] = target_state;
                /* Trig the state again with empty signal */
                (void)TRIG_STATE(target_state, &tempSignal);
            }

            /* Now we should have all the superstates in one array */
            target_state = hierarchy[0]; // save first assigned state again

            tempSignal.sig = ENTRY_SIG;
            do
            {
                /* Trig the entries of each hierarchical states */
                TRIG_STATE(hierarchy[indx], &tempSignal);
                --indx;
            } while (indx >= 0);

            t = hierarchy[0];

            tempSignal.sig = INIT_SIG;
            rtn = TRIG_STATE(t, &tempSignal); // execute initial transition

            // cState = target_state;
            // if (rtn == HANDLE_TRAN)
            //     tempSignal.sig = ENTRY_SIG;
            // else
            //     /* Trig the current state with empty event */
            //     tempSignal.sig = EMPTY_SIG;

            // rtn = TRIG_STATE(cState, &tempSignal);

        } while (rtn == HANDLE_TRAN);

        current_state = t;
        target_state = t;
    }

    void HSM::dispatcher(Event const *const e)
    {

        stateHandlerPtr t = current_state;

        stateHandlerPtr super_state;

        State rtn; // returned status

        Event tempSignal;

        do
        {
            /* Target state is being updated when we are using 
            tran() or super() functions */
            super_state = target_state;
            /* Execute the current event */
            rtn = (*super_state)(this, e);

        } while (rtn == SUPER_STATE);

        /* If the transition is wanted to be taken */
        if (rtn == HANDLE_TRAN)
        {
            stateHandlerPtr path[MAX_NEST_DEPTH];

            path[0] = target_state; // save last assigned target state
            path[1] = t;
            path[2] = super_state;

            for (; t != super_state; t = target_state)
            {
                //Trig the exit state before exiting
                tempSignal.sig = EXIT_SIG;
                if (TRIG_STATE(t, &tempSignal) == STATE_HANDLED)
                {
                    /* Find the super state by sending empty event */
                    // tempSignal{.sig = EMPTY_SIG};
                    // rtn = TRIG_STATE(super_state, &tempSignal);
                }
            }

            super_state = target_state;

            const Event evt{.sig = ENTRY_SIG};

            rtn = (*super_state)(this, &evt);
        }

        //current_state = cState;
    }

}
