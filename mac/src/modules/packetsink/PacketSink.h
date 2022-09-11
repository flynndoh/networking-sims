#ifndef __MAC_PACKETSINK_H_
#define __MAC_PACKETSINK_H_

#include <omnetpp.h>
#include "../../Utils.h"
#include "../../messages/application/AppMessage_m.h"

class PacketSink : public omnetpp::cSimpleModule {
  protected:
    // Set up simulation state.
    void initialize();

    // Tear down simulation state.
    void finish();

    // Generic message handling entry point.
    void handleMessage(omnetpp::cMessage *msg);

  private:
    // MAC layer gate ids.
    int fromMACGateId, toMACGateId;

    // Number of packets received by this packet sink.
    long numReceived;

    // Map between sender ids and the last sequence numbers this packet sink has seen from them.
    std::map<int, int> transmitterLastSeqno;

    // Specific message type handlers.
    void handleAppMessage(AppMessage *msg);
};

#endif /* __MAC_PACKETSINK_H_ */
