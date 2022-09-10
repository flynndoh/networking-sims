#ifndef __COSC441MAC_TRANSCEIVER_H_
#define __COSC441MAC_TRANSCEIVER_H_

#include <omnetpp.h>
#include "../../messages/signal/SignalStart_m.h"
#include "../../messages/signal/SignalStop_m.h"
#include "../../messages/carriersense/CSRequest_m.h"
#include "../../messages/carriersense/CSResponse_m.h"
#include "../../messages/mediumaccesscontrol/MacMessage_m.h"
#include "../../messages/transmission/TransmissionRequest_m.h"
#include "../../messages/transmission/TransmissionConfirm_m.h"
#include "../../messages/transmission/TransmissionIndication_m.h"

using namespace omnetpp;

enum TransceiverState {
    TXRX_STATE_RECEIVE  = 0,
    TXRX_STATE_TRANSMIT = 1,
    TXRX_STATE_TURNING_AROUND = 2
};

class Transceiver : public cSimpleModule {
  protected:
    void initialize();
    void finish();
    void handleMessage(cMessage *msg);
    void handleSignalStart(SignalStart *msg);
    void handleSignalStop(SignalStop *msg);
    void handleCSRequest(CSRequest *msg);
    void processCarrierSense();
    void handleTransmissionRequest(TransmissionRequest *msg);
    void processTurnAroundToTransmitCompleted();
    void processTurnAroundToReceiveCompleted();
    void processStartOfTransmission();
    void processEndOfTransmission();
    void markAllMessagesAsCollided();
    SignalStart* getTransmittingSignalWithId(int signalId, bool andRemove=false);
    double calculateBitErrorRate(double distance, double signalTxPowerDBm);
    double calculateCurrentSignalPower();
    void handleSelfMessage(cMessage *msg);

  private:
    int parentNodeId, fromMACGateId, toMACGateId, fromChannelGateId, toChannelGateId;
    int acknowledgementSize;
    double transceiverPositionX, transceiverPositionY;
    double pathLossExponent, noisePowerDBm, txPowerDBm, bitRate;
    double currentSignalPower, csThreshDBm, csTime, turnAroundTime;
    std::map<int, SignalStart*> currentTransmissions;
    cMessage* csTimeMessage = nullptr;
    cMessage* turnAroundToTransmitCompleted = nullptr;
    cMessage* turnAroundToReceiveCompleted = nullptr;
    cMessage* endOfTransmissionMessage = nullptr;
    MacMessage* currentTransmissionMacMessage = nullptr;
    TransceiverState transceiverState;
};

#endif
