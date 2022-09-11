#ifndef __MAC_PACKETSINK_H_
#define __MAC_PACKETSINK_H_

#include <omnetpp.h>
#include "../../Utils.h"
#include "../../messages/application/AppMessage_m.h"

class PacketSink : public omnetpp::cSimpleModule {
  protected:
    void initialize();
    void finish();
    void handleMessage(omnetpp::cMessage *msg);
    void handleAppMessage(AppMessage *msg);

  private:
    int fromMACGateId;
    int toMACGateId;
    long numReceived;

    std::map<int, int> transmitterLastSeqno;
};

#endif /* __MAC_PACKETSINK_H_ */
