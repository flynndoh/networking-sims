#ifndef __COSC441MAC_CHANNEL_H_
#define __COSC441MAC_CHANNEL_H_

#include <omnetpp.h>

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
