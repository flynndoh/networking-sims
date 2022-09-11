#ifndef __WLAN_RECEIVER_H_
#define __WLAN_RECEIVER_H_

#include <omnetpp.h>
#include "../../Utils.h"
#include "../../packets/ResponsePacket_m.h"

class Receiver final : public omnetpp::cSimpleModule {
  protected:
    // Set up simulation state.
    void initialize();

    // Generic message handling entry point.
    void handleMessage(omnetpp::cMessage* msg);

  private:
    // Channel gate id.
    int fromChannelGateId;
};

#endif /* __WLAN_RECEIVER_H_ */
