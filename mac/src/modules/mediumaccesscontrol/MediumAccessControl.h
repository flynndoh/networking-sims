#ifndef __MAC_MEDIUMACCESSCONTROL_H_
#define __MAC_MEDIUMACCESSCONTROL_H_

#include <omnetpp.h>
#include "../../FifoBuffer.h"
#include "../../Utils.h"
#include "../../messages/application/AppResponse_m.h"
#include "../../messages/application/AppMessage_m.h"
#include "../../messages/carriersense/CSResponse_m.h"
#include "../../messages/carriersense/CSRequest_m.h"
#include "../../messages/mediumaccesscontrol/MacMessage_m.h"
#include "../../messages/transmission/TransmissionConfirm_m.h"
#include "../../messages/transmission/TransmissionConfirmStatus_m.h"
#include "../../messages/transmission/TransmissionRequest_m.h"
#include "../../messages/transmission/TransmissionIndication_m.h"
#include "../../FifoBuffer.h"

enum MediumAccessControlState
{
    MAC_STATE_IDLE = 0,
    MAC_STATE_RUNNING_ATTEMPTS = 1,
    MAC_STATE_RUNNING_BACKOFFS = 2,
    MAC_STATE_AWAITING_BACKOFF = 3,
    MAC_STATE_AWAITING_RETRANSMISSION = 4,
    MAC_STATE_AWAITING_ACK = 5,
    MAC_STATE_AWAITING_CARRIER_SENSE = 6,
};

class MediumAccessControl final : public omnetpp::cSimpleModule {
  protected:
    // Set up simulation state.
    void initialize();

    // Tear down simulation state.
    void finish();

    // Generic message handling entry point.
    void handleMessage(omnetpp::cMessage *msg);

  private:
    int bufferSize;
    int maxBackoffs;
    int backoffCounter;
    int maxAttempts;
    int attemptCounter;

    // Transceiver gate ids.
    int fromTransceiverGateId, toTransceiverGateId;

    // PacketGenerator/PacketSink gate ids.
    int fromHigherLayerGateId, toHigherLayerGateId;

    // The amount of time to wait for an ACK packet to return.
    double ackTimeout;

    // Current state of the MAC module.
    MediumAccessControlState state;

    // Fifo buffer that contains all application layer messages waiting to be sent.
    FifoBuffer<AppMessage*> * macBuffer = nullptr;

    // Self messages.
    omnetpp::cMessage* triggerMacBufferProcess = nullptr;
    omnetpp::cMessage* backoffTimerMessage = nullptr;
    omnetpp::cMessage* retransmissionTimerMessage = nullptr;
    omnetpp::cMessage* ackTimerMessage = nullptr;

    // Used to keep track of the sequence number of the acknowledgements.
    int seqNoCounter = 0;

    // The message that we are waiting to hear back an acknowledgement from.
    MacMessage* macMessage;

    // Returns the number of seconds to use for a random backoff (in seconds).
    double getBackoffDistribution(void);

    // Returns the number of seconds to use for a retransmission backoff time (in seconds).
    double getRetransmissionDistribution(void);

    // Specific message type handlers.
    void handleSelfMessage(omnetpp::cMessage *msg);
    void handleAppMessage(AppMessage* appMessage);
    void handleMacMessage(MacMessage* macMessage);
    void handleCSResponseMessage(CSResponse* csResponse);
    void handleTransmissionConfirm(TransmissionConfirm* transmissionConfirm);
    void handleTransmissionIndication(TransmissionIndication* transmissionIndication);

    // This is the MAC module's state machine. It processes incoming messages given its current processing state.
    // The state machine frequently schedules self-messages to simulate processing delays and to continue execution
    // at future simulation times.
    void processMacBuffer(omnetpp::cMessage * msg);

    // Returns true when the given message is a valid acknowledgement message.
    bool isAcknowledgementOk(MacMessage * macMessage);
};

#endif /* __MAC_MEDIUMACCESSCONTROL_H_ */
