#ifndef AO_SERVICE_HPP
#define AO_SERVICE_HPP

#define MAX_ACTIVE 32U
#define CRIT_ENTRY_()
#define CRIT_EXIT_()

#define STATE_DEC(stateName)                   \
    State stateName##_h(Event const *const e); \
    static State stateName(void *const me, Event const *const e)

#define STATE_DEF(subclass, stateName)                              \
    State subclass::stateName(void *const me, Event const *const e) \
    {                                                               \
        return static_cast<subclass *>(me)->stateName##_h(e);       \
    }                                                               \
    State subclass::stateName##_h(Event const *const e)

namespace Event_driven
{

    using State = std::uint8_t;
    using Signal = std::uint16_t;

    struct Event
    {
        Signal sig;                    //!< signal of the event instance
        std::uint8_t poolId_;          //!< pool ID (0 for static event)
        std::uint8_t volatile refCtr_; //!< reference counter
    };

    // Forward decleration
    class Active;

    using stateHandlerPtr = State (*)(void *const me, Event const *const e);

    // Standard functions provided by framework
    class sf
    {

    public:
        sf(){}

        /* Register an active object to be managed by the framework
       This function will add the created active objects into a */
        static void add(Active *const a) noexcept
        {
            CRIT_ENTRY_();
            if (AO_index < MAX_ACTIVE)
                active_[AO_index++] = a;
            CRIT_EXIT_();

            std::cout << active_[AO_index-1] << std::endl;
        }

        static void run() noexcept
        {
            // Interrupt needs to be disabled

            // initial events need to be run
            // initial_events();

            // interrupt needs to be enabled

            for (;;)
            {
                //on_idle();
            }
        }

    private:
        static Active *active_[MAX_ACTIVE + 1U]; // to be used by QF ports only
        static size_t AO_index;
    };

    class HSM
    {

        stateHandlerPtr current_state;
        stateHandlerPtr temp_state;

    public:
        HSM() {}

        HSM(stateHandlerPtr initial_state) : temp_state{initial_state} {}

        void dispatcher(Event const *const e)
        {
            stateHandlerPtr cState = temp_state;

            stateHandlerPtr s;
            State r;

            r = (*cState)(this, e);
        }

    protected:
        State tran(stateHandlerPtr const target) noexcept
        {
            temp_state = target;
            return HANDLE_TRAN;
        }

        State super(stateHandlerPtr const superstate) noexcept
        {
            temp_state = superstate;
            return SUPER_STATE;
        }

    public:
        static constexpr State STATE_HANDLED{0};

        static constexpr State HANDLE_TRAN{1};

        static constexpr State SUPER_STATE{2};
    };

    class Active : public HSM
    {

    public:
        Active() {}

        Active(stateHandlerPtr initial_state) : HSM(initial_state) {}

        void start(std::uint_fast8_t const prio, Event const **const qSto)
        {
            std::cout << "AO has been started!\n";
            sf::add(this);
        }

    private:
        friend class sf;
    };

} // Event_driven namespace

#endif //AO_SERVICE_HPP