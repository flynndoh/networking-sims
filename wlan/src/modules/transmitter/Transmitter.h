#ifndef __WLAN_TRANSMITTER_H_
#define __WLAN_TRANSMITTER_H_

#include <omnetpp.h>
#include "../../Utils.h"
#include "../../packets/ResponsePacket_m.h"
#include "../../messages/RequestPacketMessage_m.h"

class Transmitter final : public omnetpp::cSimpleModule {
  protected:
    // Set up simulation state.
    void initialize();

    // Generic message handling entry point.
    void handleMessage(omnetpp::cMessage* msg);

private:
    // Channel gate ids.
    int fromChannelGateId, toChannelGateId;

    int sequenceNumber = 0;
    int64_t overheadBits;
    int64_t userBits;

    // Create a simple response packet to be sent to the channel.
    ResponsePacket* createResponsePacket();
};

#endif /* __WLAN_TRANSMITTER_H_ */
