#include "hsm.hpp"

namespace Event_driven
{

    void HSM::dispatcher(Event const *const e)
    {
        stateHandlerPtr cState = temp_state;

        stateHandlerPtr s;
        State r;

        r = (*cState)(this, e);
    }

}
