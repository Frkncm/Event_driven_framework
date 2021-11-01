#include "hsm.hpp"

namespace Event_driven
{

    void HSM::dispatcher(Event const *const e)
    {
        stateHandlerPtr cState = current_state;

        stateHandlerPtr s;
        State rtn; // returned status

        rtn = (*cState)(this, e);

        if (rtn == HANDLE_TRAN)
        {
            /* if transition has been taken */
            current_state = target_state;
            dispatcher(nullptr);
        }
    }

}
