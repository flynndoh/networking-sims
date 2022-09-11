#ifndef __MAC_CHANNEL_H_
#define __MAC_CHANNEL_H_

#include <omnetpp.h>
#include <stdlib.h>
#include "../../Utils.h"
#include "../../messages/signal/SignalStart_m.h"
#include "../../messages/signal/SignalStop_m.h"

class Channel : public omnetpp::cSimpleModule {
  protected:
    void initialize();
    void finish();
    void handleMessage(omnetpp::cMessage *msg);

  private:
    int fromReceiverGateId, toReceiverGateId;
    int *toTransmitterGateIds, *fromTransmitterGateIds;
    int numTransmitters;

    void handleSignalMessage(omnetpp::cMessage *msg);
};

#endif /* __MAC_CHANNEL_H_ */
