#ifndef __WLAN_RECEIVER_H_
#define __WLAN_RECEIVER_H_

#include <omnetpp.h>
#include "../../packets/ResponsePacket_m.h"

class Receiver : public omnetpp::cSimpleModule {
  public:
    void initialize();
    void handleMessage(omnetpp::cMessage* msg);
  private:
    int inGateId;
};

#endif /* __WLAN_RECEIVER_H_ */
