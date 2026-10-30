/*
Copyright (C) 2026 Michael Schultz, Bremen-Germany

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


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
