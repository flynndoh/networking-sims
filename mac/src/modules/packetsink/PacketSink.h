#ifndef __mac_PACKETSINK_H_
#define __mac_PACKETSINK_H_

#include <omnetpp.h>
#include "../../messages/application/AppMessage_m.h"

using namespace omnetpp;

class PacketSink : public cSimpleModule {
  protected:
    void initialize();
    void finish();
    void handleMessage(cMessage *msg);
    void handleAppMessage(AppMessage *msg);

  private:
    int fromMACGateId;
    int toMACGateId;
    long numReceived;

    std::map<int, int> transmitterLastSeqno;
};

#endif /* __mac_PACKETSINK_H_ */
