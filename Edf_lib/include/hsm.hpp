#ifndef _HSM_HPP_
#define _HSM_HPP_

#include <cstdint>

#define STATE_DEC(stateName)                                               \
    Event_driven::State stateName##_h(Event_driven::Event const *const e); \
    static Event_driven::State stateName(void *const me, Event_driven::Event const *const e)

#define STATE_DEF(subclass, stateName)                                                          \
    Event_driven::State subclass::stateName(void *const me, Event_driven::Event const *const e) \
    {                                                                                           \
        return static_cast<subclass *>(me)->stateName##_h(e);                                   \
    }                                                                                           \
    Event_driven::State subclass::stateName##_h(Event_driven::Event const *const e)

namespace Event_driven
{

    using State = std::uint8_t;
    using Signal = std::uint16_t;

    struct Event
    {
        Signal sig; // signal of the event instance
        //std::uint8_t poolId_;          // pool ID (0 for static event)
        //std::uint8_t volatile refCtr_; // reference counter
    };

    using stateHandlerPtr = State (*)(void *const me, Event const *const e);

    class HSM
    {

        stateHandlerPtr current_state;
        stateHandlerPtr target_state;

    public:
        HSM() {}

        HSM(stateHandlerPtr initial_state)
        {
            //Register the initial state
            current_state = initial_state;
        }

        void dispatcher(Event const *const e);

    protected:
        State tran(stateHandlerPtr const target) noexcept
        {
            target_state = target;
            return HANDLE_TRAN;
        }

        State super(stateHandlerPtr const superstate) noexcept
        {
            target_state = superstate;
            return SUPER_STATE;
        }

    public:
        static constexpr State STATE_HANDLED{0};

        static constexpr State HANDLE_TRAN{1};

        static constexpr State SUPER_STATE{2};

    protected:

        enum ReservedHsmSignals : Signal
        {
            ENTRY_SIG = 1, // signal for entry actions
            EXIT_SIG,      // signal for exit actions
            INIT_SIG       // signal for nested initial transitions
        };
    };

}

#endif // _HSM_HPP_