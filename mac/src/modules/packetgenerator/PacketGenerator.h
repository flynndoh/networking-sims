#ifndef __MAC_PACKETGENERATOR_H_
#define __MAC_PACKETGENERATOR_H_

#include <omnetpp.h>
#include "../../Utils.h"
#include "../../messages/application/AppResponse_m.h"
#include "../../messages/application/AppMessage_m.h"

class PacketGenerator final : public omnetpp::cSimpleModule {
  protected:
    // Set up simulation state.
    void initialize();

    // Tear down simulation state.
    void finish();

    // Generic message handling entry point.
    void handleMessage(omnetpp::cMessage *msg);

  private:
    // MAC Layer gate ids.
    int toMacGateId, fromMacGateId;

    int senderId, receiverId;
    long seqNo = 0;
    long numSent;

    // A self message that is used to trigger the sending of an AppMessage to the MAC layer.
    omnetpp::cMessage* readyMessage = nullptr;

    // Returns the parameter for the inter-arrival time distribution (in seconds).
    double getIatDistribution(void);

    // Returns the parameter for the message size distribution (in bytes).
    double getMsgSizeDistribution(void);

    // Sends the AppMessage after a random amount of time.
    void sendAppMessage(void);

    // The handler for AppResponse messages.
    void handleAppResponse(AppResponse* appResponse);
};

#endif /* __MAC_PACKETGENERATOR_H_ */
