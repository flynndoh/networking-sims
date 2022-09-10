#ifndef __WLAN_SIMULATION_RECEIVER_H_
#define __WLAN_SIMULATION_RECEIVER_H_

#include <omnetpp.h>

using namespace omnetpp;

class Receiver : public cSimpleModule {
  public:
    void initialize();
    void handleMessage(cMessage* msg);
  private:
    int inGateId;
};

#endif /* __WLAN_SIMULATION_RECEIVER_H_ */
