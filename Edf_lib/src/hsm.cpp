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
            tempSignal.sig = EXIT_SIG;
            (void)TRIG_STATE(s, &tempSignal);
            ip = 0; // enter the target
        }
        else
        {
            tempSignal.sig = EMPTY_SIG;
            (void)TRIG_STATE(t, &tempSignal); // superstate of target
            t = target_state;

            // (b) check source==target->super
            if (s == t)
            {
                ip = 0; // enter the target
            }
            else
            {
                tempSignal.sig = EMPTY_SIG;
                (void)TRIG_STATE(t, &tempSignal); // superstate of target
                // (c) check source->super==target->super
                if (target_state == t)
                {
                    tempSignal.sig = EXIT_SIG;
                    (void)TRIG_STATE(s, &tempSignal); // exit the source
                    ip = 0;                           // enter the target
                }
                else
                {
                    // (d) check source->super==target
                    if (target_state == hierarchy[0])
                    {
                        tempSignal.sig = EXIT_SIG;
                        (void)TRIG_STATE(s, &tempSignal); // exit the source
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
                        tempSignal.sig = EMPTY_SIG;
                        r = TRIG_STATE(hierarchy[1], &tempSignal);

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
                                tempSignal.sig = EMPTY_SIG;
                                r = TRIG_STATE(target_state, &tempSignal);
                            }
                        }

                        if (iq == 0)
                        {

                            tempSignal.sig = EXIT_SIG;
                            (void)TRIG_STATE(target_state, &tempSignal);

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
                                    tempSignal.sig = EXIT_SIG;
                                    if (TRIG_STATE(t, &tempSignal) == STATE_HANDLED)
                                    {
                                        tempSignal.sig = EMPTY_SIG;
                                        (void)TRIG_STATE(t, &tempSignal);
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

        Event tempSignal;

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
                tempSignal.sig = EXIT_SIG;
                if (TRIG_STATE(t, &tempSignal) == STATE_HANDLED)
                {
                    /* Find the super state by sending empty event */
                    tempSignal.sig = EMPTY_SIG;
                    (void)TRIG_STATE(source_state, &tempSignal);
                }
            }

            std::int_fast8_t ip = hsm_tran(path);

            // retrace the entry path in reverse (desired) order...
            tempSignal.sig = ENTRY_SIG;
            for (; ip >= 0; --ip)
            {
                TRIG_STATE(path[ip], &tempSignal);
            }
            t = path[0];      // stick the target into register
            target_state = t; // update the next state

            // drill into the target hierarchy...
            tempSignal.sig = INIT_SIG;
            while (TRIG_STATE(t, &tempSignal) == HANDLE_TRAN)
            {
                ip = 0;
                path[0] = target_state;

                tempSignal.sig = EMPTY_SIG;
                (void)TRIG_STATE(target_state, &tempSignal); // find superstate

                while (target_state != t)
                {
                    ++ip;
                    path[ip] = target_state;
                    tempSignal.sig = EMPTY_SIG;
                    (void)TRIG_STATE(target_state, &tempSignal); // find superstate
                }
                target_state = path[0];

                // retrace the entry path in reverse (correct) order...
                tempSignal.sig = ENTRY_SIG;
                do
                {
                    TRIG_STATE(path[ip], &tempSignal);
                    --ip;
                } while (ip >= 0);

                t = path[0];
            }
        }

        current_state = t; // change the current active state
        target_state = t;  // mark the configuration as stable
    }
}
