#ifndef __MAC_CHANNEL_H_
#define __MAC_CHANNEL_H_

#include <omnetpp.h>
#include <stdlib.h>
#include "../../Utils.h"
#include "../../messages/signal/SignalStart_m.h"
#include "../../messages/signal/SignalStop_m.h"

class Channel : public omnetpp::cSimpleModule {
  protected:
    // Set up simulation state.
    void initialize();

    // Tear down simulation state.
    void finish();

    // Generic message handling entry point.
    void handleMessage(omnetpp::cMessage *msg);

  private:
    // Input and output gate ids.
    int fromReceiverGateId, toReceiverGateId;
    int *toTransmitterGateIds, *fromTransmitterGateIds;

    // Number of transmitters on the channel, used to find and assign ids to all gates.
    int numTransmitters;

    // Specific message type handler.
    void handleSignalMessage(omnetpp::cMessage *msg);
};

#endif /* __MAC_CHANNEL_H_ */
