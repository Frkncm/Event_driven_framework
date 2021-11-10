#include "hsm.hpp"
#include <assert.h>
//! helper macro to trigger internal event in an HSM

static Event_driven::Event const frameworkEvents_[4]{
    {0U}, // EMPTY_SIG
    {1U}, // ENTRY_SIG
    {2U}, // EXIT_SIG
    {3U}  // INIT_SIG
};

#define TRIG_STATE(state_, sig_) ((*(state_))(this, &frameworkEvents_[sig_]))

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
            (void)TRIG_STATE(target_state, EMPTY_SIG);

            while (target_state != t)
            {
                /* Check if target is top() function. When we hit the top
                function, we are in superstate. */
                ++indx;
                /* Save superstate of each states */
                hierarchy[indx] = target_state;
                /* Trig the state again with empty signal */
                (void)TRIG_STATE(target_state, EMPTY_SIG);
            }

            /* Now we should have all the superstates in one array */
            target_state = hierarchy[0]; // save first assigned state again

            do
            {
                /* Trig the entries of each hierarchical states */
                (void)TRIG_STATE(hierarchy[indx], ENTRY_SIG);
                --indx;
            } while (indx >= 0);

            t = hierarchy[0];

            rtn = TRIG_STATE(t, INIT_SIG); // execute initial transition

        } while (rtn == HANDLE_TRAN);

        current_state = t;
        target_state = t;
    }

    std::int_fast8_t HSM::hsm_tran(stateHandlerPtr *hierarchy)
    {
        std::int_fast8_t ip = -1; // transition entry path index
        std::int_fast8_t iq;      // helper transition entry path index
        stateHandlerPtr t = hierarchy[0];
        stateHandlerPtr const s = hierarchy[2];
        State r;
        Event tempSignal;

        // (a) check source==target (transition to self)
        if (s == t)
        {
            (void)TRIG_STATE(s, EXIT_SIG);
            ip = 0; // enter the target
        }
        else
        {
            (void)TRIG_STATE(t, EMPTY_SIG); // superstate of target
            t = target_state;

            // (b) check source==target->super
            if (s == t)
            {
                ip = 0; // enter the target
            }
            else
            {
                (void)TRIG_STATE(t, EMPTY_SIG); // superstate of target
                // (c) check source->super==target->super
                if (target_state == t)
                {
                    (void)TRIG_STATE(s, EXIT_SIG); // exit the source
                    ip = 0;                        // enter the target
                }
                else
                {
                    // (d) check source->super==target
                    if (target_state == hierarchy[0])
                    {
                        (void)TRIG_STATE(s, EXIT_SIG); // exit the source
                    }
                    else
                    {
                        // (e) check rest of source==target->super->super..
                        // and store the entry path along the way

                        iq = 0;           // indicate that the LCA was not found
                        ip = 1;           // enter target and its superstate
                        hierarchy[1] = t; // save the superstate of target
                        t = target_state; // save source->super

                        // find target->super->super
                        r = TRIG_STATE(hierarchy[1], EMPTY_SIG);

                        while (r == SUPER_STATE)
                        {
                            ++ip;
                            hierarchy[ip] = target_state; // store the entry path
                            if (target_state == s)
                            { // is it the source?
                                // indicate that the LCA was found
                                iq = 1;

                                --ip;              // do not enter the source
                                r = STATE_HANDLED; // terminate the loop
                            }
                            // it is not the source, keep going up
                            else
                            {
                                r = TRIG_STATE(target_state, EMPTY_SIG);
                            }
                        }

                        if (iq == 0)
                        {
                            (void)TRIG_STATE(target_state, EXIT_SIG);

                            // (f) check the rest of source->super
                            //                  == target->super->super...
                            //
                            iq = ip;
                            r = IGNORED_EVENT; // indicate LCA NOT found

                            do
                            {
                                // is this the LCA?
                                if (t == hierarchy[iq])
                                {
                                    r = STATE_HANDLED; // indicate LCA found
                                    ip = iq - 1;       // do not enter LCA
                                    iq = -1;           // cause termination of the loop
                                }
                                else
                                {
                                    --iq; // try lower superstate of target
                                }
                            } while (iq >= 0);

                            if (r != STATE_HANDLED)
                            {
                                // (g) check each source->super->...
                                // for each target->super...
                                //
                                r = IGNORED_EVENT; // keep looping
                                do
                                {
                                    // exit t unhandled?
                                    if (TRIG_STATE(t, EXIT_SIG) == STATE_HANDLED)
                                    {
                                        (void)TRIG_STATE(t, EMPTY_SIG);
                                    }
                                    t = target_state; //  set to super of t
                                    iq = ip;
                                    do
                                    {
                                        // is this LCA?
                                        if (t == hierarchy[iq])
                                        {
                                            ip = iq - 1;       // do not enter LCA
                                            iq = -1;           // break out of inner loop
                                            r = STATE_HANDLED; // break from outer loop
                                        }
                                        else
                                        {
                                            --iq;
                                        }
                                    } while (iq >= 0);
                                } while (r != STATE_HANDLED);
                            }
                        }
                    }
                }
            }
        }
        return ip;
    }

    void HSM::dispatcher(Event const *const e)
    {

        stateHandlerPtr t = current_state;

        stateHandlerPtr source_state;

        State rtn; // returned status

        /* Let the execute posted event in hieratical manner */
        do
        {
            /* Target state is being updated when we are using 
            tran() or super() functions */
            source_state = target_state;
            /* Execute the current event */
            rtn = (*source_state)(this, e);

        } while (rtn == SUPER_STATE);

        /* If the transition is wanted to be taken */
        if (rtn == HANDLE_TRAN)
        {
            stateHandlerPtr path[MAX_NEST_DEPTH];

            path[0] = target_state; // save last assigned target state
            path[1] = t;
            path[2] = source_state;

            for (; t != source_state; t = target_state)
            {
                //Trig the exit state before exiting
                if (TRIG_STATE(t, EXIT_SIG) == STATE_HANDLED)
                {
                    /* Find the super state by sending empty event */
                    (void)TRIG_STATE(source_state, EMPTY_SIG);
                }
            }

            std::int_fast8_t ip = hsm_tran(path);

            // retrace the entry path in reverse (desired) order...
            for (; ip >= 0; --ip)
            {
                (void)TRIG_STATE(path[ip], ENTRY_SIG);
            }
            t = path[0];      // stick the target into register
            target_state = t; // update the next state

            // drill into the target hierarchy...
            while (TRIG_STATE(t, INIT_SIG) == HANDLE_TRAN)
            {
                ip = 0;
                path[0] = target_state;

                (void)TRIG_STATE(target_state, EMPTY_SIG); // find superstate

                while (target_state != t)
                {
                    ++ip;
                    path[ip] = target_state;
                    (void)TRIG_STATE(target_state, EMPTY_SIG); // find superstate
                }
                target_state = path[0];

                // retrace the entry path in reverse (correct) order...
                do
                {
                    (void)TRIG_STATE(path[ip], ENTRY_SIG);
                    --ip;
                } while (ip >= 0);

                t = path[0];
            }
        }

        current_state = t; // change the current active state
        target_state = t;  // mark the configuration as stable
    }
}
