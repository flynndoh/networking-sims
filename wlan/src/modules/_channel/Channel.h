#ifndef __WLAN_SIMULATION_CHANNEL_H_
#define __WLAN_SIMULATION_CHANNEL_H_

#include <omnetpp.h>
#include "../../packets/ResponsePacket_m.h"
#include "../../messages/PacketCompletionMessage_m.h"
#include "../../messages/RequestPacketMessage_m.h"

using namespace omnetpp;

class Channel : public cSimpleModule {
    public:
      void initialize ();
      void handleMessage(cMessage* msg);
      void handleCompletion();
      void handleIncomingPacket(ResponsePacket* pkt);
      double calculateBitErrorRate();
      double determineGain(bool channelState);
      double calculatePathLoss(double distanceMeters, double pathLossExp);
      double calculateReceivedPower(double txPowerDBm, double gain, double pathLossDB);
      double calculateSNRRelation(double receivedPowerDBm, double noisePowerDBm, double bitRate);
      double normalToDecibels(double target);
      double decibelsToNormal(double target);
      ~Channel();
    protected:
      double bitRate, pathLossExponent, nodeDistance, txPowerDBm, noisePowerDBm;
      double channelGainGoodDB, channelGainBadDB, txProbabilityGoodGood, txProbabilityBadBad;
      int fromTransmitterGateId, toTransmitterGateId, toReceiverGateId;
      cMessage* requestNewPacket;
      RequestPacketMessage* newPacketRequest;
      PacketCompletionMessage* completion;
      ResponsePacket* currentPacket;
};

#endif /* __WLAN_SIMULATION_CHANNEL_H_ */
