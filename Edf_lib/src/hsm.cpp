#include "hsm.hpp"

namespace Event_driven
{

    void HSM::dispatcher(Event const *const e)
    {
        stateHandlerPtr cState = current_state;

        State rtn; // returned status

        do
        {
            const Event evt{.sig = ENTRY_SIG};
            rtn = (*cState)(this, &evt);
            cState = current_state = target_state;
            /* if transition has been taken */
        } while (rtn == HANDLE_TRAN);
    }

}
