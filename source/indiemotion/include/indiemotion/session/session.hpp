#pragma once

#include <indiemotion/common.hpp>
#include <indiemotion/errors.hpp>
#include <indiemotion/motion/controller.hpp>
#include <indiemotion/motion/xform.hpp>
#include <indiemotion/server/server.hpp>
#include <indiemotion/session/delegate.hpp>
#include <indiemotion/session/motion_mode.hpp>
#include <indiemotion/session/properties.hpp>
#include <indiemotion/session/state.hpp>
#include <indiemotion/version.hpp>

namespace indiemotion::session
{

    // Forward Declaration
    class SessionManager;

    class Session
    {

        friend class SessionManager;

    private:
        std::shared_ptr<SessionDelegate> _m_sessionDelegate = nullptr;
        std::shared_ptr<motion::MotionController> _m_motionController = nullptr;
        std::shared_ptr<state::State> _m_state = nullptr;
        std::shared_ptr<motion::ModeController> _m_motionModeController = nullptr;

    public:
        // Default Constructor
        Session()
        {
            _initializeState();
            _initializeMotion();
        };

        Session(std::shared_ptr<SessionDelegate> sessionDelegate, std::shared_ptr<motion::MotionDelegate> motionDelegate)
            : Session()
        {
            _m_sessionDelegate = sessionDelegate;
            _m_motionController->bindDelegate(motionDelegate);
        }

        // Copy the resource (copy constructor)
        // We do not allow for the Sesion Object ot be copied
        Session(const Session &rhs) = delete;

        // Transfer Ownership (move constructor)
        Session(Session &&rhs) noexcept
        {
            _m_sessionDelegate = std::exchange(rhs._m_sessionDelegate, nullptr);
            _m_state = std::exchange(rhs._m_state, nullptr);
        }

        // Make type `std::swap`able
        friend void swap(Session &a, Session &b) noexcept
        {
            a.swap(b);
        }

        // Destructor
        ~Session() {}

        // Assignment by Value
        Session &operator=(Session copy)
        {
            copy.swap(*this);
            return *this;
        }

        void swap(Session &rhs) noexcept
        {
            using std::swap;
            swap(_m_sessionDelegate, rhs._m_sessionDelegate);
            swap(_m_state, rhs._m_state);
        }

        /**
         * @brief Bind the delegate to the session
         * 
         * @param delegate 
         */
        void bindSessionDelegate(std::shared_ptr<SessionDelegate> delegate)
        {
            _m_sessionDelegate = delegate;
        }

        /**
         * @brief Bind the delegate to the motion controller
         * 
         * @param delegate 
         */
        void bindMotionDelegate(std::shared_ptr<motion::MotionDelegate> delegate)
        {
            _m_motionController->bindDelegate(delegate);
        }

        /**
         * @brief Initialize the session for motion capture
         * 
         */
        void initialize()
        {
            auto properties = Properties{
                // TODO Make Constant
                "indiemotion-server",
                indiemotion::API_VERSION,
                FeatureSet(0)};

            if (_m_sessionDelegate)
            {
                _m_sessionDelegate->sessionWillInitialize();

                if (auto name = _m_sessionDelegate->name())
                {
                    properties.name = *name;
                }

                if (auto features = _m_sessionDelegate->supportedFeatures())
                {
                    properties.features = *features;
                }
            }
            _m_state->set(session::state::Key::Properties, properties);
            _m_state->set(session::state::Key::Status, session::state::SessionStatus::Initializing);
        }

        /**
         * @brief Returns the current properties for the session.
         * 
         * @return Properties 
         */
        Properties properties() const noexcept
        {
            return _m_state->get<Properties>(session::state::Key::Properties);
        }

        /**
         * @brief Returns a pointer to the session state.
         * 
         * @return std::shared_ptr<state::State> 
         */
        std::shared_ptr<state::State> state() const noexcept
        {
            return _m_state;
        }

        /**
         * @brief Return the current motion controller
         * 
         * @return std::shared_ptr<motion::MotionController> 
         */
        std::shared_ptr<motion::MotionController> motionController() const noexcept
        {
            return _m_motionController;
        }

        /**
         * @brief Activate the current session
         * 
         */
        void activate()
        {
            _m_state->set(state::Key::Status, state::SessionStatus::Active);
            if (_m_sessionDelegate)
            {
                _m_sessionDelegate->sessionDidInitialize();
            }
        }

        /**
         * @brief Current Status of the session
         * 
         * @return state::SessionStatus 
         */
        state::SessionStatus status() const noexcept
        {
            return _m_state->get<state::SessionStatus>(state::Key::Status);
        }

        /**
         * @brief Return whether the current session is active
         * 
         * @return true 
         * @return false 
         */
        bool isActive() const noexcept
        {
            return _m_state->get<state::SessionStatus>(state::Key::Status) == state::SessionStatus::Active;
        }

        /**
         * @brief Return the current list of camera names
         * 
         * @return std::vector<std::string> 
         */
        std::vector<std::string> cameras() const
        {
            _checkIsActive();

            if (_m_sessionDelegate)
            {
                return _m_sessionDelegate->cameras();
            }
            else
            {
                return std::vector<std::string>();
            }
        }

        /**
         * @brief Returns the current motion mode set in this session
         * 
         * @return motion::ModeValue 
         */
        motion::ModeValue motionMode() const
        {
            return _m_motionModeController->current();
        }

        /**
         * @brief Update the current mode to the given on
         * 
         * @param mode 
         */
        void updateMotionMode(motion::ModeValue mode)
        {
            if (_m_motionModeController->current() == mode)
            {
                return; // No updates needed.
            }

            switch (mode)
            {
            case motion::ModeValue::Off:
                _m_motionModeController->off();
                break;

            case motion::ModeValue::Live:
                _m_motionModeController->live();
                break;

            case motion::ModeValue::Recording:
                _m_motionModeController->record();
                break;
            }

            if (_m_sessionDelegate)
            {
                _m_sessionDelegate->motionModeDidUpdate(mode);
            }
        }

        /**
         * @brief Update the current motion transform
         * 
         * @param xform 
         */
        void update(std::unique_ptr<motion::MotionXForm> xform)
        {
            _checkIsActive();
            _m_motionController->update(std::move(xform));
        }

    private:
        /**
         * @brief Initialize the state object on this class
         * 
         */
        void _initializeState()
        {
            _m_state = std::make_shared<state::State>();
            _m_state->set(state::Key::Status, state::SessionStatus::Inactive);
        }

        /**
         * @brief Initialize the Motion Mode for the Session
         * 
         */
        void _initializeMotion()
        {
            _m_motionModeController = motion::ModeController::create();
            _m_motionController = std::make_shared<motion::MotionController>();
        }

        void _checkIsActive() const
        {
            if (!isActive())
            {
                throw indiemotion::errors::SessionError(
                    "com.indiemotion.error.sessionNotInitialized",
                    "session cannot perform action until the session is activated.");
            }
        }
    };

}