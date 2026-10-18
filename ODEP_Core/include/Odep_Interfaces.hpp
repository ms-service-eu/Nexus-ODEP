#pragma once
#include "Odep_Core.hpp"

namespace Odep {

    class IStreamListener {
    public:
        virtual ~IStreamListener() = default;
        virtual void onFrameIngest(const OdepFrame& frame) = 0;
    };

    class ITransportFacade {
    public:
        virtual ~ITransportFacade() = default;
        virtual bool transmit(const OdepFrame& frame) = 0;
        virtual void attachListener(IStreamListener* listener) = 0;
    };

    class IProcessingElement : public IStreamListener {
    protected:
        EntityId          m_element_id;
        ITransportFacade& m_network;
    public:
        IProcessingElement(EntityId id, ITransportFacade& net) : m_element_id(id), m_network(net) {}
        virtual ~IProcessingElement() = default;
        virtual void evaluate(const PtpTimestamp& system_time) = 0;
    };
}
