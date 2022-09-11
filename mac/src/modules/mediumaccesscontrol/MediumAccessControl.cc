#include "MediumAccessControl.h"

Define_Module(MediumAccessControl);

omnetpp::simsignal_t bufferDropped = omnetpp::cComponent::registerSignal("bufferDropped");
omnetpp::simsignal_t lostInChannel = omnetpp::cComponent::registerSignal("lostInChannel");

void MediumAccessControl::initialize() {
    // Extract simulation parameters from NED file.
    bufferSize = par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    maxAttempts = par("maxAttempts");
    ackTimeout = par("ackTimeout");

    // Extract simulation gates from NED file.
    fromTransceiverGateId = findGate("fromTransceiver");
    assert(fromTransceiverGateId != -1);
    toTransceiverGateId = findGate("toTransceiver");
    assert(toTransceiverGateId != -1);
    fromHigherLayerGateId = findGate("fromHigherLayer");
    assert(fromHigherLayerGateId != -1);
    toHigherLayerGateId = findGate("toHigherLayer");
    assert(toHigherLayerGateId != -1);

    // Create the FIFO buffer.
    macBuffer = new FifoBuffer<AppMessage*>(bufferSize);
    assert(macBuffer != nullptr);

    // Set up self-messages.
    triggerMacBufferProcess = new omnetpp::cMessage("triggerMacBufferProcess");
    assert(triggerMacBufferProcess != nullptr);

    ackTimerMessage = new omnetpp::cMessage("ackTimerMessage");
    assert(ackTimerMessage != nullptr);

    backoffTimerMessage = new omnetpp::cMessage("backoffTimerMessage");
    assert(backoffTimerMessage != nullptr);

    retransmissionTimerMessage = new omnetpp::cMessage("retransmissionTimerMessage");
    assert(retransmissionTimerMessage != nullptr);

    state = MAC_STATE_IDLE;
    macMessage = nullptr;
}

void MediumAccessControl::finish() {
    // Clean up the buffer.
    while (!macBuffer->isEmpty()) {
        AppMessage *appMessage = macBuffer->pop();
        delete appMessage;
    }
    delete macBuffer;

    // Clean up self-messages.
    cancelAndDelete(triggerMacBufferProcess);
    triggerMacBufferProcess = nullptr;

    cancelAndDelete(ackTimerMessage);
    ackTimerMessage = nullptr;

    cancelAndDelete(backoffTimerMessage);
    backoffTimerMessage = nullptr;

    cancelAndDelete(retransmissionTimerMessage);
    retransmissionTimerMessage = nullptr;

    if (macMessage != nullptr) {
        delete macMessage;
    }
}

double MediumAccessControl::getBackoffDistribution(void) {
    return par("backoffDistribution");
}

double MediumAccessControl::getRetransmissionDistribution(void) {
    return par("retransmissionDistribution");
}

void MediumAccessControl::handleSelfMessage(omnetpp::cMessage *msg) {
    if (msg == triggerMacBufferProcess) {
        EV << HERE << "INFO: received self-message triggerMacBufferProcess." << std::endl;
        processMacBuffer(msg);
        return;
    }

    if (msg == ackTimerMessage) {
        EV << HERE << "INFO: received self-message ackTimerMessage." << std::endl;
        processMacBuffer(msg);
        return;
    }

    if (msg == retransmissionTimerMessage) {
        EV << HERE << "INFO: received self-message retransmissionTimerMessage." << std::endl;
        processMacBuffer(msg);
        return;
    }

    if (msg == backoffTimerMessage) {
        EV << HERE << "INFO: received self-message backoffTimerMessage." << std::endl;
        processMacBuffer(msg);
        return;
    }

    EV << HERE << "ERROR: self-message is of unexpected type." << std::endl;
    error("Shouldn't get here");
    delete msg; // Prevent memory leaks.
}

void MediumAccessControl::handleMessage(omnetpp::cMessage *msg) {
    if (msg->isSelfMessage()) {
        return handleSelfMessage(msg);
    }

    tryHandleMessage(msg, AppMessage*, fromHigherLayerGateId, handleAppMessage);
    tryHandleMessage(msg, TransmissionConfirm*, fromTransceiverGateId, handleTransmissionConfirm);
    tryHandleMessage(msg, MacMessage*, fromTransceiverGateId, handleMacMessage);
    tryHandleMessage(msg, CSResponse*, fromTransceiverGateId, handleCSResponseMessage);
    tryHandleMessage(msg, TransmissionIndication*, fromTransceiverGateId,handleTransmissionIndication)

    // If we get here we have received a message that we didn't expect :(
    EV << HERE << "ERROR: msg is an unexpected type." << std::endl;
    error("Shouldn't get here");
    delete msg;
}

void MediumAccessControl::handleAppMessage(AppMessage *appMessage) {
    AppResponse *appResponse = nullptr;

    try {
        // Attempt to insert the AppMessage into the FIFO buffer.
        macBuffer->push(appMessage);

        EV << HERE << "INFO: pushed AppMessage to macBuffer." << std::endl;

        // Emit statistic for packets not dropped at the buffer.
        emit(bufferDropped, false);

        // Process the buffer now.
        if (state == MAC_STATE_IDLE) { // only process the mac buffer if we aren't in the middle of doing it already
            scheduleAt(omnetpp::simTime(), triggerMacBufferProcess);
        }
    } catch (FifoBufferFullException *ex) {
        // If the buffer is full, drop the message and send an AppResponse message to the higher layer.
        delete appMessage;
        delete ex;

        EV << HERE << "INFO: could not push AppMessage to macBuffer. Buffer full." << std::endl;

        // Emit statistic for packets dropped at the buffer.
        emit(bufferDropped, true);

        appResponse = new AppResponse();
        appResponse->setSuccess(false);

        send(appResponse, toHigherLayerGateId);
    } catch (std::exception ex) {
        error("Shouldn't get here");
    }
}

void MediumAccessControl::handleCSResponseMessage(CSResponse *csResponse) {
    processMacBuffer(csResponse);
}


void MediumAccessControl::handleTransmissionConfirm(TransmissionConfirm *transmissionConfirm) {
    emit(lostInChannel, transmissionConfirm->getStatus() != statusOk);
    delete transmissionConfirm;
}

void MediumAccessControl::processMacBuffer(omnetpp::cMessage *msg) {
    omnetpp::simtime_t now = omnetpp::simTime();
    AppResponse *appResponse = nullptr;

    switch (state) {
        case MAC_STATE_IDLE: {
            if (!macBuffer->isEmpty()) {
                // Get the oldest AppMessage from buffer and encapsulate it into a MacMessage.
                AppMessage *appMessage = macBuffer->pop();
                assert(appMessage != nullptr);

                assert(macMessage == nullptr);
                macMessage = new MacMessage();
                macMessage->setOriginalSeqNo(-1);
                macMessage->setSeqNo(seqNoCounter);
                macMessage->setPacketLengthBits(appMessage->getMsgSize() * 8);
                macMessage->setReceiverNodeId(appMessage->getReceiverId());
                macMessage->setTransmitterNodeId(appMessage->getSenderId());
                macMessage->encapsulate(appMessage);

                // Increment sequence number, reset attempt counter and attempt to transmit.
                seqNoCounter++;
                attemptCounter = 0;
                state = MAC_STATE_RUNNING_ATTEMPTS;
                processMacBuffer(nullptr);
            } else {
                EV << HERE << "No more packets to process!" << std::endl;
            }
            break;
        }
        case MAC_STATE_RUNNING_ATTEMPTS: {
           if (attemptCounter < maxAttempts) {
               // While we still have > 0 transmit attempts, repeatedly attempt to transmit MacMessage.
               attemptCounter++;
               backoffCounter = 0;
               state = MAC_STATE_RUNNING_BACKOFFS;
               processMacBuffer(nullptr);
           } else {
               // Exhausted all transmit attempts, report failure to higher layer and cleanup MacMessage.
               appResponse = new AppResponse();
               appResponse->setSuccess(false);
               send(appResponse, toHigherLayerGateId);

               delete macMessage;
               emit(lostInChannel, true);

               state = MAC_STATE_IDLE;
               processMacBuffer(nullptr);
           }
           break;
        }
        case MAC_STATE_RUNNING_BACKOFFS: {
            if (backoffCounter < maxBackoffs) {
                // If we haven't been backed off, send a carrier sense request to the Transceiver
                backoffCounter++;
                state = MAC_STATE_AWAITING_CARRIER_SENSE;
                send(new CSRequest(), toTransceiverGateId);
                EV << HERE << "INFO: Sent a CSRequest!" << std::endl;
            } else {
                // Darn, we should wait some simulation time before attempting again.
                state = MAC_STATE_AWAITING_BACKOFF;
                scheduleAt(now + getBackoffDistribution(), backoffTimerMessage);
            }
            break;
        }
        case MAC_STATE_AWAITING_CARRIER_SENSE: {
            CSResponse *csRes = nullptr;
            assert(dynamic_cast<CSResponse*>(msg) != nullptr);
            csRes = (CSResponse*) msg;

            if (csRes->getBusyChannel()) {
                // Channel is busy, we should retry again after the retransmission backoff timer has elapsed.
                state = MAC_STATE_AWAITING_RETRANSMISSION;
                scheduleAt(now + getRetransmissionDistribution(), retransmissionTimerMessage);
                EV << HERE << "INFO: Channel busy. Waiting retransmission backoff." << std::endl;
            } else {
                // Channel is free! Send a transmission request to the Transceiver.

                assert(macMessage != nullptr);
                TransmissionRequest *trRequest = new TransmissionRequest();

                // Make a copy of the MacMessage and encapsulate it in the transmission request.
                trRequest->encapsulate(macMessage->dup());

                EV << HERE << "Sending TransmissionRequest to Transceiver" << std::endl;
                send(trRequest, toTransceiverGateId);

                // Reset backoff counter, and await for ack.
                backoffCounter = 0;
                state = MAC_STATE_AWAITING_ACK;
                scheduleAt(now + ackTimeout, ackTimerMessage);
            }
            delete msg; // Tidy kiwi.
            break;
        }
        case MAC_STATE_AWAITING_ACK: {
            if (msg == ackTimerMessage) {
                // Ack timeout, we should try and retransmit.
                state = MAC_STATE_AWAITING_RETRANSMISSION;
                scheduleAt(now + getRetransmissionDistribution(), retransmissionTimerMessage);
            } else {
                assert(dynamic_cast<MacMessage*>(msg) != nullptr);
                MacMessage* macMessage = (MacMessage*) msg;
                assert(macMessage->getType() == MAC_PACKET_TYPE_ACK);

                if (isAcknowledgementOk(macMessage)) {
                    // Message transmission has been confirmed successful, as indicated by ack from transceiver.

                    cancelEvent(ackTimerMessage); // Cancel ack timer, but don't delete it as we need it when sending subsequent messages.

                    // Send the good news to the higher layer and cleanup.
                    appResponse = new AppResponse();
                    appResponse->setSuccess(true);
                    send(appResponse, toHigherLayerGateId);
                    delete macMessage;
                    state = MAC_STATE_IDLE;
                    processMacBuffer(nullptr);
                } else {
                    // Received someone else's ack message or the ACK message was stale.
                    EV << HERE << "INFO: Dropped an ACK message that we don't care about." << std::endl;
                }
                delete msg; // Tidy kiwi.
            }
            break;
        }
        case MAC_STATE_AWAITING_BACKOFF: {
            if (msg == backoffTimerMessage) {
                // Await backoff has completed, move to next state.
                state = MAC_STATE_RUNNING_BACKOFFS;
                processMacBuffer(nullptr);
            }
            break;
        }
        case MAC_STATE_AWAITING_RETRANSMISSION: {
            if (msg == retransmissionTimerMessage) {
                // Await retransmission has completed, move to next state.
                state = MAC_STATE_RUNNING_ATTEMPTS;
                processMacBuffer(nullptr);
            }
            break;
        }
        default: {
            error("Shouldn't get here as the state machine should be exhaustive. Is there some memory corruption going on?");
            break;
        }
    }
}

bool MediumAccessControl::isAcknowledgementOk(MacMessage *macMessage) {
    assert(macMessage != nullptr);
    return ((macMessage->getTransmitterNodeId() == macMessage->getReceiverNodeId())
        && (macMessage->getReceiverNodeId() == macMessage->getTransmitterNodeId())
        && (macMessage->getOriginalSeqNo() == macMessage->getSeqNo()));
}

void MediumAccessControl::handleMacMessage(MacMessage *macMessage) {
    omnetpp::cPacket *pkt = nullptr;
    AppMessage *appMessage = nullptr;

    switch (macMessage->getType()) {
        case MAC_PACKET_TYPE_ACK: {
            if (state == MAC_STATE_AWAITING_ACK)
            {
                EV << HERE << "Got an ACK packet!" << std::endl;
                processMacBuffer(macMessage);
            }
            else
            {
                EV << HERE << "Warning: Dropping an ACK packet because the MAC isn't in the right state!" << std::endl;
                delete macMessage;
            }
            break;
        }
        case MAC_PACKET_TYPE_DATA: {
            // Decapsulate and send to higher layer.
            pkt = macMessage->decapsulate();
            assert(pkt != nullptr);
            assert(dynamic_cast<AppMessage*>(pkt) != nullptr);

            appMessage = (AppMessage*) pkt;
            send(appMessage, toHigherLayerGateId);
            EV << HERE << "INFO: Passed AppMessage to higher layer." << std::endl;

            delete macMessage; // Cleanup.
            break;
        }
        default: {
            error("Shouldn't get here. Exhaustive switch statement. Could indicate memory corruption.");
            break;
        }
    }
}

void MediumAccessControl::handleTransmissionIndication(
    TransmissionIndication *transmissionIndication) {
    omnetpp::cPacket *pkt = nullptr;

    pkt = transmissionIndication->decapsulate();
    assert(pkt != nullptr);

    assert(dynamic_cast<MacMessage*>(pkt) != nullptr);
    MacMessage *macMessage = (MacMessage*) pkt;

    EV << HERE << "Decapsulating TransmissionIndication to MacMessage" << std::endl;

    delete transmissionIndication;
    handleMacMessage(macMessage);
}

