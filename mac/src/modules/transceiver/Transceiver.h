#ifndef __MAC_TRANSCEIVER_H_
#define __MAC_TRANSCEIVER_H_

#include <omnetpp.h>
#include "../../Utils.h"
#include "../../MathHelpers.h"
#include "../../messages/signal/SignalStart_m.h"
#include "../../messages/signal/SignalStop_m.h"
#include "../../messages/carriersense/CSRequest_m.h"
#include "../../messages/carriersense/CSResponse_m.h"
#include "../../messages/mediumaccesscontrol/MacMessage_m.h"
#include "../../messages/transmission/TransmissionRequest_m.h"
#include "../../messages/transmission/TransmissionConfirm_m.h"
#include "../../messages/transmission/TransmissionIndication_m.h"

enum TransceiverState {
    TXRX_STATE_RECEIVE  = 0,
    TXRX_STATE_TRANSMIT = 1,
    TXRX_STATE_TURNING_AROUND = 2
};

class Transceiver final : public omnetpp::cSimpleModule {
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

    // Channel gate ids.
    int fromChannelGateId, toChannelGateId;

    // Used to filter addressee's.
    int parentNodeId;

    // Used to simulate the effects of different ack message sizes in bits.
    int acknowledgementSize;

    // The cartesian position of the transceiver.
    double transceiverPositionX, transceiverPositionY;

    // Transmit power fields.
    double pathLossExponent, noisePowerDBm, txPowerDBm, bitRate;

    // Carrier sense fields.
    double currentSignalPower, csThreshDBm, csTime, turnAroundTime;

    // Map between transceiver id and their signal (if transmitting).
    std::map<int, SignalStart*> currentTransmissions;

    // Self messages.
    omnetpp::cMessage* csTimeMessage = nullptr;
    omnetpp::cMessage* turnAroundToTransmitCompleted = nullptr;
    omnetpp::cMessage* turnAroundToReceiveCompleted = nullptr;
    omnetpp::cMessage* endOfTransmissionMessage = nullptr;

    // The MacMessage to be transmitted by this transceiver over the channel.
    MacMessage* currentTransmissionMacMessage = nullptr;

    // Current state of the Transceiver's internal state machine.
    TransceiverState transceiverState;

    // Message processing functions.
    void processCarrierSense();
    void processTurnAroundToTransmitCompleted();
    void processTurnAroundToReceiveCompleted();
    void processStartOfTransmission();
    void processEndOfTransmission();

    // Iterate through all messages on the channel that are currently being transmitted, and mark them all as collided.
    void markAllMessagesAsCollided();

    // Fetch a given signal by its id.
    SignalStart* getTransmittingSignalById(int signalId, bool andRemove=false);

    // Calculate the expected bit error rate given the transceivers simulation parameters.
    double calculateBitErrorRate(double distance, double signalTxPowerDBm);

    // Listen to the carrier/channel and determine how much signal power is currently present. This provides
    // an indication as to how busy the channel is, and is used to determine if the Transceiver should
    // begin Transmission operations.
    double calculateCurrentSignalPower();

    // Specific message type handlers.
    void handleSelfMessage(omnetpp::cMessage *msg);
    void handleSignalStart(SignalStart *msg);
    void handleSignalStop(SignalStop *msg);
    void handleCSRequest(CSRequest *msg);
    void handleTransmissionRequest(TransmissionRequest *msg);
};

#endif /* __MAC_TRANSCEIVER_H_ */
