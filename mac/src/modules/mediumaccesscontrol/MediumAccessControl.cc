#include "MediumAccessControl.h"

Define_Module(MediumAccessControl);

simsignal_t bufferDropped = cComponent::registerSignal("bufferDropped");
simsignal_t lostInChannel = cComponent::registerSignal("lostInChannel");

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
    triggerMacBufferProcess = new cMessage("triggerMacBufferProcess");
    assert(triggerMacBufferProcess != nullptr);

    ackTimerMessage = new cMessage("ackTimerMessage");
    assert(ackTimerMessage != nullptr);

    backoffTimerMessage = new cMessage("backoffTimerMessage");
    assert(backoffTimerMessage != nullptr);

    retransmissionTimerMessage = new cMessage("retransmissionTimerMessage");
    assert(retransmissionTimerMessage != nullptr);

    state = MAC_STATE_IDLE;
    macMessage = nullptr;
}

void MediumAccessControl::finish() {
    // Clean up the buffer.
    try {
        while (1) {
            AppMessage *appMessage = macBuffer->pop();
            delete appMessage;
        }
    } catch (FifoBufferEmptyException *ex) {
        // Ignore the error we get when we cannot take any more items from the buffer.
        delete ex;
    }

    delete macBuffer;
    macBuffer = nullptr;

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
        macMessage = nullptr;
    }
}

double MediumAccessControl::getBackoffDistribution(void) {
    return par("backoffDistribution");
}

double MediumAccessControl::getRetransmissionDistribution(void) {
    return par("retransmissionDistribution");
}

void MediumAccessControl::handleSelfMessage(cMessage *msg) {
    if (msg == triggerMacBufferProcess) {
        EV << HERE << "INFO: received self-message triggerMacBufferProcess." << endl;
        processMacBuffer(msg);
        return;
    }

    if (msg == ackTimerMessage) {
        EV << HERE << "INFO: received self-message ackTimerMessage." << endl;
        processMacBuffer(msg);
        return;
    }

    if (msg == retransmissionTimerMessage) {
        EV << HERE << "INFO: received self-message retransmissionTimerMessage." << endl;
        processMacBuffer(msg);
        return;
    }

    if (msg == backoffTimerMessage) {
        EV << HERE << "INFO: received self-message backoffTimerMessage." << endl;
        processMacBuffer(msg);
        return;
    }

    EV << HERE << "ERROR: self-message is of unexpected type." << endl;
    error("Shouldn't get here");
    delete msg; // Prevent memory leaks.
}

void MediumAccessControl::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        return handleSelfMessage(msg);
    }

    tryHandleMessage(msg, AppMessage*, fromHigherLayerGateId, handleAppMessage);
    tryHandleMessage(msg, TransmissionConfirm*, fromTransceiverGateId, handleTransmissionConfirm);
    tryHandleMessage(msg, MacMessage*, fromTransceiverGateId, handleMacMessage);
    tryHandleMessage(msg, CSResponse*, fromTransceiverGateId, handleCSResponseMessage);
    tryHandleMessage(msg, TransmissionIndication*, fromTransceiverGateId,handleTransmissionIndication)

    // If we get here we have received a message that we didn't expect :(
    EV << HERE << "ERROR: msg is an unexpected type." << endl;
    error("Shouldn't get here");
    delete msg;
}

void MediumAccessControl::handleAppMessage(AppMessage *appMessage) {
    AppResponse *appResponse = nullptr;

    try {
        // Attempt to insert the AppMessage into the FIFO buffer.
        macBuffer->push(appMessage);

        EV << HERE << "INFO: pushed AppMessage to macBuffer." << endl;

        // Emit statistic for packets not dropped at the buffer.
        emit(bufferDropped, false);

        // Process the buffer now.
        if (state == MAC_STATE_IDLE) { // only process the mac buffer if we aren't in the middle of doing it already
            scheduleAt(simTime(), triggerMacBufferProcess);
        }
    } catch (FifoBufferFullException *ex) {
        // If the buffer is full, drop the message and send an AppResponse message to the higher layer.
        delete appMessage;
        delete ex;

        EV << HERE << "INFO: could not push AppMessage to macBuffer. Buffer full." << endl;

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
    if (transmissionConfirm->getStatus() == statusOk)
        emit(lostInChannel, false);
    else {
        emit(lostInChannel, true);
    }

    // Cleanup.
    delete transmissionConfirm;
}

void MediumAccessControl::processMacBuffer(cMessage *msg) {
    simtime_t now = simTime();
    AppResponse *appResponse = nullptr;

    switch (state) {
        case MAC_STATE_IDLE: {
            if (!macBuffer->empty()) {
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

                // Increment sequence number counter.
                seqNoCounter++;

                attemptCounter = 0;

                state = MAC_STATE_RUNNING_ATTEMPTS;
                processMacBuffer(nullptr);
            } else {
                EV << HERE << "No more packets to process!" << endl;
            }
            break;
        }
        case MAC_STATE_RUNNING_ATTEMPTS: {
           if (attemptCounter < maxAttempts) {
               attemptCounter++;

               backoffCounter = 0;

               state = MAC_STATE_RUNNING_BACKOFFS;
               processMacBuffer(nullptr);
           } else {
               appResponse = new AppResponse();
               appResponse->setSuccess(false);
               send(appResponse, toHigherLayerGateId);

               delete macMessage;
               macMessage = nullptr;

               emit(lostInChannel, true);

               state = MAC_STATE_IDLE;
               processMacBuffer(nullptr);
           }
           break;
        }
        case MAC_STATE_RUNNING_BACKOFFS: {
           if (backoffCounter < maxBackoffs) {
               backoffCounter++;

               state = MAC_STATE_AWAITING_CARRIER_SENSE;
               send(new CSRequest(), toTransceiverGateId);
               EV << HERE << "INFO: Sent a CSRequest!" << endl;
           } else {
               state = MAC_STATE_AWAITING_BACKOFF;
               scheduleAt(now + getBackoffDistribution(), backoffTimerMessage);
           }
           break;
        }
        case MAC_STATE_AWAITING_CARRIER_SENSE: {
            CSResponse *csRes = nullptr;
           assert(dynamic_cast<CSResponse*>(msg) != nullptr);
           csRes = (CSResponse*) msg;

           if (csRes->getBusyChannel() == false) {
               assert(macMessage != nullptr);

               TransmissionRequest *trRequest = new TransmissionRequest();

               // Make a copy of the MacMessage and encapsulate it in the transmission request.
               trRequest->encapsulate(macMessage->dup());

               EV << HERE << "Sending TransmissionRequest to Transceiver" << endl;
               send(trRequest, toTransceiverGateId);
               backoffCounter = 0;
               state = MAC_STATE_AWAITING_ACK;

               scheduleAt(now + ackTimeout, ackTimerMessage);
           } else {
               state = MAC_STATE_AWAITING_RETRANSMISSION;
               scheduleAt(now + getRetransmissionDistribution(),
                       retransmissionTimerMessage);
               EV << HERE << "INFO: Channel busy. Waiting retransmission backoff."
                         << endl;
           }
           delete msg; // Tidy kiwi
           break;
        }
        case MAC_STATE_AWAITING_ACK: {
            if (msg == ackTimerMessage) {
                state = MAC_STATE_AWAITING_RETRANSMISSION;
                scheduleAt(now + getRetransmissionDistribution(),
                        retransmissionTimerMessage);
            } else {
                assert(dynamic_cast<MacMessage*>(msg) != nullptr);
                MacMessage* macMessage = (MacMessage*) msg;

                assert(macMessage->getType() == MAC_PACKET_TYPE_ACK);

                if (isAcknowledgementOk(macMessage)) {
                    cancelEvent(ackTimerMessage); // Don't delete as we need it later.

                    appResponse = new AppResponse();
                    appResponse->setSuccess(true);
                    send(appResponse, toHigherLayerGateId);

                    delete macMessage;
                    macMessage = nullptr;

                    state = MAC_STATE_IDLE;
                    processMacBuffer(nullptr);
                } else {
                    // Received someone else's ack message or the ACK message was stale.
                    EV << HERE << "INFO: Dropped an ACK message that we don't care about."
                              << endl;
                }
                delete msg; // Tidy kiwi.
            }
            break;
        }
        case MAC_STATE_AWAITING_BACKOFF: {
            if (msg == backoffTimerMessage) {
                state = MAC_STATE_RUNNING_BACKOFFS;
                processMacBuffer(nullptr);
            }
            break;
        }
        case MAC_STATE_AWAITING_RETRANSMISSION: {
            if (msg == retransmissionTimerMessage) {
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
    assert(macMessage != nullptr);
    return ((macMessage->getTransmitterNodeId() == macMessage->getReceiverNodeId())
            && (macMessage->getReceiverNodeId() == macMessage->getTransmitterNodeId())
            && (macMessage->getOriginalSeqNo() == macMessage->getSeqNo()));
}

void MediumAccessControl::handleMacMessage(MacMessage *macMessage) {
    cPacket *pkt = nullptr;
    AppMessage *appMessage = nullptr;

    switch (macMessage->getType()) {
        case MAC_PACKET_TYPE_ACK: {
            if (state == MAC_STATE_AWAITING_ACK)
            {
                EV << HERE << "Got an ACK packet!" << endl;
                processMacBuffer(macMessage);
            }
            else
            {
                EV << HERE << "Warning: Dropping an ACK packet because the MAC isn't in the right state!" << endl;
                delete macMessage;
            }
            break;
        }
        case MAC_PACKET_TYPE_DATA: {
            // Decapsulate and send to higher layer
            pkt = macMessage->decapsulate();
            assert(pkt != nullptr);
            assert(dynamic_cast<AppMessage*>(pkt) != nullptr);

            appMessage = (AppMessage*) pkt;
            send(appMessage, toHigherLayerGateId);
            EV << HERE << "INFO: Passed AppMessage to higher layer." << endl;

            delete macMessage; // clean up memory
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
    cPacket *pkt = nullptr;

    pkt = transmissionIndication->decapsulate();
    assert(pkt != nullptr);

    assert(dynamic_cast<MacMessage*>(pkt) != nullptr);
    MacMessage *macMessage = (MacMessage*) pkt;

    EV << HERE << "Decapsulating TransmissionIndication to MacMessage" << endl;

    delete transmissionIndication;
    handleMacMessage(macMessage);
}

