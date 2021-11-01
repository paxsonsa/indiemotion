#pragma once
#include <indiemotion/common.hpp>
#include <indiemotion/net/acknowledge.hpp>
#include <indiemotion/net/camera.hpp>
#include <indiemotion/net/message.hpp>
#include <indiemotion/net/motion.hpp>
#include <indiemotion/session/handler.hpp>
#include <indiemotion/session/properties.hpp>
#include <indiemotion/session/session.hpp>

namespace indiemotion::session
{
    class SessionBridge
    {
    private:
        std::shared_ptr<Session> _m_sessionPtr;
        std::unique_ptr<net::AcknowledgeCoordinator> _m_ackCoordinator;

    public:
        SessionBridge(std::shared_ptr<Session> sessionPtr)
        {
            _m_sessionPtr = sessionPtr;
            _m_ackCoordinator = std::make_unique<net::AcknowledgeCoordinator>();
        }

        std::unique_ptr<net::Message> initialize()
        {
            // TODO Return Initialize
            auto payload = std::make_unique<SessionProperties>(
                "fakeserver",
                "1.0",
                newFeatureSet(0));
            _m_sessionPtr->setStatus(Status::Initialized);
            auto message = net::createMessage(std::move(payload));
            message->requiresAcknowledgement(true);

            _m_ackCoordinator->queue(message->id(),
                                     [&]()
                                     { _m_sessionPtr->setStatus(Status::Activated); });

            return std::move(message);
        }

        std::optional<std::unique_ptr<net::Message>> processMessage(std::unique_ptr<net::Message> messagePtr) const
        {
            std::optional<std::unique_ptr<net::Message>> response;
            switch (messagePtr->payloadType())
            {
            case net::PayloadType::Acknowledge:
            {
                if (messagePtr->inResponseToId())
                {
                    _m_ackCoordinator->acknowledge(messagePtr->inResponseToId().value());
                }
                // TODO Handle Malformed Acknowledge

                return {};
            }

                // ---------------------------------------------------------------------------------------------------
                // Camera Operations

            case net::PayloadType::GetCameraList:
            {
                auto cameras = _m_sessionPtr->getCameras();
                auto payload = std::make_unique<net::CameraList>(std::move(cameras));
                auto response = net::createMessage(messagePtr->id(), std::move(payload));
                return response;
            }

            case net::PayloadType::SetCamera:
            {
                auto msgPayload = messagePtr->payloadPtrAs<net::SetCamera>();
                _m_sessionPtr->setActiveCamera(msgPayload->cameraId);
                auto camera = _m_sessionPtr->getActiveCamera();
                auto payload = std::make_unique<net::CameraInfo>(camera.value());
                auto response = net::createMessage(messagePtr->id(), std::move(payload));
                return response;
            }

                // ---------------------------------------------------------------------------------------------------
                // Motion Mode Operations

            case net::PayloadType::SetMotionMode:
            {
                auto msgPayload = messagePtr->payloadPtrAs<net::SetMotionMode>();
                _m_sessionPtr->setMotionMode(msgPayload->mode);
                return {};
            }

            case net::PayloadType::SessionInitilization:
            case net::PayloadType::SessionShutdown: // TODO how do we shut down
            case net::PayloadType::Error:           // TODO Process Errors
                return {};

            default:
                // TODO Send Error
                return {};
            }
        }
    };
}