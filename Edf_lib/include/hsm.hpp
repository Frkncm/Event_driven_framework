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

#define POST(ao_, evt_) do{ Event_driven::Event t_evt_ {.sig = evt_};\
                                   ao_->post_(&t_evt_);} while(false) 

namespace Event_driven
{
    using State = std::uint8_t;
    using Signal = std::uint16_t;

    constexpr Signal USER_SIG{4};

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
        HSM() = delete;

        HSM(stateHandlerPtr initial_state)
        {
            //Current state should be assigned to empty state
            current_state = reinterpret_cast<stateHandlerPtr>(&top);
            //Register the initial state
            target_state = initial_state;
        }

        void init(void);

        std::int_fast8_t hsm_tran(stateHandlerPtr *hierarchy);

        void dispatcher(Event const *const e);

        static State top(void *const me, Event const *const e) noexcept;

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

        static constexpr State IGNORED_EVENT{4};

    protected:
        enum ReservedHsmSignals : Signal
        {
            ENTRY_SIG = 1, // signal for entry actions
            EXIT_SIG,      // signal for exit actions
            INIT_SIG       // signal for nested initial transitions
        };

    private:
        static constexpr std::int_fast8_t MAX_NEST_DEPTH{6};
    };

}

#endif // _HSM_HPP_