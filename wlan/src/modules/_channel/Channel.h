#ifndef __WLAN_CHANNEL_H_
#define __WLAN_CHANNEL_H_

#include <omnetpp.h>
#include <cmath>
#include "../../MathHelpers.h"
#include "../../Utils.h"
#include "../../packets/ResponsePacket_m.h"
#include "../../messages/PacketCompletionMessage_m.h"
#include "../../messages/RequestPacketMessage_m.h"

class Channel : public omnetpp::cSimpleModule {
  protected:
    // Set up simulation state.
    void initialize();

    // Tear down simulation state.
    void finish();

    // Generic message handling entry point.
    void handleMessage(omnetpp::cMessage* msg);

    // Specific message handling.
    void handleCompletion();
    void handleIncomingPacket(ResponsePacket* pkt);

    // Computes the bit error rate using the complementary error function for each bit.
    double calculateBitErrorRate();

    // Determines the appropriate gain value (i.e. GOOD or BAD), depending on the current state of the channel.
    double determineGain(bool channelState);

  private:
    // Transmit power fields.
    double bitRate, pathLossExponent, nodeDistance, txPowerDBm, noisePowerDBm;

    // Carrier sense fields.
    double channelGainGoodDB, channelGainBadDB, txProbabilityGoodGood, txProbabilityBadBad;

    // Channel gate ids.
    int fromTransmitterGateId, toTransmitterGateId;

    // Channel gate ids.
    int toReceiverGateId;

    // Messages.
    omnetpp::cMessage* requestNewPacket;
    RequestPacketMessage* newPacketRequest;
    PacketCompletionMessage* completion;
    ResponsePacket* currentPacket;
};

#endif /* __WLAN_CHANNEL_H_ */
