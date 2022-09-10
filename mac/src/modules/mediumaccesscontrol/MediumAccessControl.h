#ifndef __COSC441MAC_MEDIUMACCESSCONTROL_H_
#define __COSC441MAC_MEDIUMACCESSCONTROL_H_

#include <omnetpp.h>

#include "../../messages/application/AppMessage_m.h"
#include "../../messages/transmission/TransmissionConfirm_m.h"
#include "../../messages/transmission/TransmissionIndication_m.h"
#include "../../messages/mediumaccesscontrol/MacMessage_m.h"
#include "../../messages/carriersense/CSResponse_m.h"
#include "../../FifoBuffer.h"

using namespace omnetpp;

enum MediumAccessControlState
{
    /**
     * The MAC isn't doing anything in particular.
     */
    MAC_STATE_IDLE = 0,

    /**
     * The MAC is running an attempt.
     */
    MAC_STATE_RUNNING_ATTEMPTS,

    /**
     * The MAC is running a backoff.
     */
    MAC_STATE_RUNNING_BACKOFFS,

    /**
     * The MAC is waiting for a backoff timer self-message.
     */
    MAC_STATE_AWAITING_BACKOFF,

    /**
     * The MAC is waiting for a retransmission timer self-message.
     */
    MAC_STATE_AWAITING_RETRANSMISSION,

    /**
     * The MAC is awaiting an ACK packet or a ack timer self-message.
     */
    MAC_STATE_AWAITING_ACK,

    /**
     * The MAC is awaiting the result of a carrier sense operation.
     */
    MAC_STATE_AWAITING_CARRIER_SENSE,

};

class MediumAccessControl : public cSimpleModule {
  protected:
   void initialize();
   void finish();
   void handleMessage(cMessage *msg);

  private:
    /**
     * The bufferSize parameter.
     */
    int bufferSize;

    /**
     * The maxBackoffs parameter.
     */
    int maxBackoffs;

    /**
     * The maxAttempts parameter.
     */
    int maxAttempts;

    /**
     * The gate ids for the Transceiver.
     */
    int fromTransceiverGateId, toTransceiverGateId;

    /**
     * The gate ids for the PacketGenerator/PacketSink.
     */
    int fromHigherLayerGateId, toHigherLayerGateId;

    /**
     * The amount of time to wait for an ACK packet to return.
     */
    double ackTimeout;

    /**
     * The state of the module.
     */
    MediumAccessControlState state;

    /**
     * The current number of attempts.
     */
    int attemptCounter;

    /**
     * The current number of backoffs.
     */
    int backoffCounter;

    cMessage* triggerMacBufferProcess = nullptr;
    cMessage* backoffTimerMessage = nullptr;
    cMessage* retransmissionTimerMessage = nullptr;
    cMessage* ackTimerMessage = nullptr;

    FifoBuffer<AppMessage*> * macBuffer = nullptr;

    /**
     * Used to keep track of the sequence number of the acknowledgements.
     */
    int seqNoCounter = 0;

    /**
     * The message that we are waiting to hear back an acknowledgement from.
     */
    MacMessage* macMessage;

    /**
     * Returns the number of seconds to use for a random backoff (in seconds).
     */
    double getBackoffDistribution(void);

    /**
     * Returns the number of seconds to use for a retransmission backoff time (in seconds).
     */
    double getRetransmissionDistribution(void);

    void handleSelfMessage(cMessage *msg);

    void handleAppMessage(AppMessage* appMessage);

    void handleMacMessage(MacMessage* macMessage);

    void handleCSResponseMessage(CSResponse* csResponse);

    void handleTransmissionConfirm(TransmissionConfirm* transmissionConfirm);

    void handleTransmissionIndication(TransmissionIndication* transmissionIndication);

    void processMacBuffer(cMessage * msg);

    bool isAcknowledgementOk(MacMessage * macMessage);
};

#endif
