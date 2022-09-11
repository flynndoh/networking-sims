#ifndef __WLAN_TRANSMITTER_H_
#define __WLAN_TRANSMITTER_H_

#include <omnetpp.h>
#include "../../packets/ResponsePacket_m.h"
#include "../../messages/RequestPacketMessage_m.h"

class Transmitter : public omnetpp::cSimpleModule {
  public:
    void initialize();
    void handleMessage(omnetpp::cMessage* msg);

    ResponsePacket* createResponsePacket();
  private:
    int64_t overheadBits;
    int64_t userBits;
    int sequenceNumber = 0;
    int toChannelGateId;
    int fromChannelGateId;
};

#endif /* __WLAN_TRANSMITTER_H_ */
