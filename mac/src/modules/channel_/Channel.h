#ifndef __MAC_CHANNEL_H_
#define __MAC_CHANNEL_H_

#include <omnetpp.h>
#include "../../Utils.h"
#include "../../messages/signal/SignalStart_m.h"
#include "../../messages/signal/SignalStop_m.h"

using namespace omnetpp;

class Channel : public cSimpleModule {
  protected:
    void initialize();
    void finish();
    void handleMessage(cMessage *msg);

  private:
    int fromReceiverGateId, toReceiverGateId;
    int *toTransmitterGateIds, *fromTransmitterGateIds;
    int numTransmitters;

    void handleSignalMessage(cMessage *msg);
};

#endif
