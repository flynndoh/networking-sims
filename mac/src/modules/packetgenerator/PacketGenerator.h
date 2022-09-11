#ifndef __MAC_PACKETGENERATOR_H_
#define __MAC_PACKETGENERATOR_H_

#include <omnetpp.h>
#include "../../Utils.h"
#include "../../messages/application/AppResponse_m.h"
#include "../../messages/application/AppMessage_m.h"

class PacketGenerator : public omnetpp::cSimpleModule {
  protected:
    void initialize();
    void finish();
    void handleMessage(omnetpp::cMessage *msg);

  private:

    /**
     * The gate ids to and from the MAC layer.
     */
    int toMacGateId, fromMacGateId;

    /**
     * The id of the sender.
     */
    int senderId;

    /**
     * The id of the receiver.
     */
    int receiverId;

    /**
     * The sequence number.
     */
    long seqNo = 0;

    long numSent;

    /**
     * Used to trigger the sending of an AppMessage
     */
    omnetpp::cMessage* readyMessage = nullptr;

    /**
     * Returns the parameter for the inter-arrival time distribution (in seconds).
     */
    double getIatDistribution(void);

    /**
     * Returns the parameter for the message size distribution (in bytes).
     */
    double getMsgSizeDistribution(void);

    /**
     * The handler for AppResponse messages.
     */
    void handleAppResponse(AppResponse* appResponse);

    /**
     * Sends the AppMessage after an amount of time.
     */
    void sendAppMessage(void);
};

#endif /* __MAC_PACKETGENERATOR_H_ */
