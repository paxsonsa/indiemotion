#pragma once
#include <indiemotion/common.hpp>
#include <indiemotion/logging.hpp>
#include <indiemotion/net/acknowledge.hpp>
#include <indiemotion/net/camera.hpp>
#include <indiemotion/net/error.hpp>
#include <indiemotion/net/message.hpp>
#include <indiemotion/net/dispatcher.hpp>
#include <indiemotion/net/motion.hpp>
#include <indiemotion/session/properties.hpp>
#include <indiemotion/session/session.hpp>

namespace indiemotion {
    const std::string LOGGER_NAME = "com.indiemotion.session.bridge";
    class SessionBridge {
    private:
        std::shared_ptr<spdlog::logger> _logger;
        std::string _m_id = "default";
        std::shared_ptr<NetMessageDispatcher> _m_dispatcher;
        std::shared_ptr<SessionController> _m_sessionPtr;
        std::unique_ptr<net::AcknowledgeCoordinator> _m_ackCoordinator;

    public:
        static const std::string APIVersion;

        SessionBridge(std::shared_ptr<NetMessageDispatcher> dispatcherPtr,
                      std::shared_ptr<SessionController> sessionPtr) {
            _m_dispatcher = std::move(dispatcherPtr);
            _logger = logging::getLogger(LOGGER_NAME);
            _m_sessionPtr = std::move(sessionPtr);
            _m_ackCoordinator = std::make_unique<net::AcknowledgeCoordinator>();
        }

        SessionBridge(std::string name,
                      std::shared_ptr<NetMessageDispatcher> dispatcherPtr,
                      std::shared_ptr<SessionController> sessionPtr) {
            _m_dispatcher = std::move(dispatcherPtr);
            _logger = logging::getLogger(LOGGER_NAME);
            _m_id = std::move(name);
            _m_sessionPtr = std::move(sessionPtr);
            _m_ackCoordinator = std::make_unique<net::AcknowledgeCoordinator>();
        }

        [[nodiscard]] static std::string apiVersion() { return SessionBridge::APIVersion; }

        void initialize() {
            auto payload = std::make_unique<SessionProperties>(
                _m_id,
                apiVersion(),
                newFeatureSet(0));

            _m_sessionPtr->setStatus(SessionStatus::Initialized);
            auto message = netMakeMessage(std::move(payload));
            message->requiresAcknowledgement(true);

            _m_ackCoordinator->queue(message->id(),
                                     [&]() { _m_sessionPtr->setStatus(SessionStatus::Activated); });

            _m_dispatcher->dispatch(std::move(message));
        }

        void processMessage(std::unique_ptr<NetMessage> messagePtr) const {
            _logger->trace("Processing message: {}", messagePtr->id());

            switch (messagePtr->payloadType()) {
            case NetPayloadType::Acknowledge: {
                _logger->trace("NetPayloadType=Acknowledge");
                if (messagePtr->inResponseToId()) {
                    _m_ackCoordinator->acknowledge(messagePtr->inResponseToId().value());
                } else {
                    auto error = std::make_unique<net::Error>(
                        net::Error::Type::InvalidMessage,
                        "acknowledgement message missing id for which message it should acknowledge");
                    auto response = netMakeMessageWithResponseID(messagePtr->id(), std::move(error));
                    _m_dispatcher->dispatch(std::move(response));
                }
                return;
            }

            // ---------------------------------------------------------------------------------------------------
            // Camera Operations
            case NetPayloadType::GetCameraList: {
                _logger->trace("NetPayloadType=GetCameraList");
                auto cameras = _m_sessionPtr->getCameras();
                auto payload = std::make_unique<net::CameraList>(std::move(cameras));
                auto response = netMakeMessageWithResponseID(messagePtr->id(), std::move(payload));
                _m_dispatcher->dispatch(std::move(response));
                return;
            }

            case NetPayloadType::SetCamera: {
                _logger->trace("NetPayloadType=SetCamera");
                auto msgPayload = messagePtr->payloadPtrAs<net::SetCamera>();
                _m_sessionPtr->setActiveCamera(msgPayload->cameraId);
                auto camera = _m_sessionPtr->getActiveCamera();
                auto payload = std::make_unique<net::CameraInfo>(camera.value());
                auto response = netMakeMessageWithResponseID(messagePtr->id(), std::move(payload));
                _m_dispatcher->dispatch(std::move(response));
                return;
            }

            // ---------------------------------------------------------------------------------------------------
            // Motion Mode Operations
            case NetPayloadType::MotionSetMode: {
                _logger->trace("NetPayloadType=MotionSetMode");
                auto msgPayload = messagePtr->payloadPtrAs<net::MotionSetMode>();
                _m_sessionPtr->setMotionMode(msgPayload->mode);
                return;
            }

                // ---------------------------------------------------------------------------------------------------
                // Motion XForm Operations
            case NetPayloadType::MotionUpdateXForm: {
                _logger->trace("NetPayloadType=MotionUpdateXForm");
                auto msgPayload = messagePtr->payloadPtrAs<net::MotionUpdateXForm>();
                _m_sessionPtr->updateMotionXForm(msgPayload->xform);
                return;
            }

                // ---------------------------------------------------------------------------------------------------
                // Generic Operations
            case NetPayloadType::Error:_logger->trace("NetPayloadType=Error");
                // TODO Process Errors
                return;

            case NetPayloadType::Unknown: {
                _logger->trace("NetPayloadType=Unknown");
                auto error = std::make_unique<net::Error>(net::Error::Type::CannotProcessMessage,
                                                          "could not process message, unknown payload type.");
                auto response = netMakeMessageWithResponseID(messagePtr->id(), std::move(error));
                _m_dispatcher->dispatch(std::move(response));
                return;
            }

            default:_logger->trace("NetPayloadType=default");
                auto error = std::make_unique<net::Error>(net::Error::Type::CannotProcessMessage,
                                                          "could not process message, handler is not implemented to process contents.");
                auto response = netMakeMessageWithResponseID(messagePtr->id(), std::move(error));
                _logger->error("could not process message '{}', payload type handler not implemented: {}",
                               messagePtr->id(),
                               messagePtr->payloadType());
                _m_dispatcher->dispatch(std::move(response));
                return;
            }
        }
    };

    const std::string SessionBridge::APIVersion = "1.0";
}