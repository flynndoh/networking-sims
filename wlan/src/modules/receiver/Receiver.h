#ifndef __WLAN_RECEIVER_H_
#define __WLAN_RECEIVER_H_

#include <omnetpp.h>
#include "../../packets/ResponsePacket_m.h"

using namespace omnetpp;

class Receiver : public cSimpleModule {
  public:
    void initialize();
    void handleMessage(cMessage* msg);
  private:
    int inGateId;
};

#endif /* __WLAN_RECEIVER_H_ */
