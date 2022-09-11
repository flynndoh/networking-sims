#ifndef __WLAN_TRANSMITTER_H_
#define __WLAN_TRANSMITTER_H_

#include <omnetpp.h>
#include "../../packets/ResponsePacket_m.h"
#include "../../messages/RequestPacketMessage_m.h"

using namespace omnetpp;

class Transmitter : public cSimpleModule {
  public:
    void initialize();
    void handleMessage(cMessage* msg);

    ResponsePacket* createResponsePacket();
  private:
    int64_t overheadBits;
    int64_t userBits;
    int sequenceNumber = 0;
    int toChannelGateId;
    int fromChannelGateId;
};

#endif /* TRANSMITTER_H_ */
