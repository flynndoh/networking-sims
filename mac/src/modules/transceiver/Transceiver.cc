#include "Transceiver.h"

Define_Module(Transceiver);

void Transceiver::initialize() {
    // Extract simulation parameters from NED file.
    parentNodeId = par("nodeId");
    transceiverPositionX = par("nodeXPosition");
    transceiverPositionY = par("nodeYPosition");
    pathLossExponent = par("pathLossExponent");
    noisePowerDBm = par("noisePowerDBm");
    txPowerDBm = par("txPowerDBm");
    bitRate = par("bitRate");
    csTime = par("csTime");
    csThreshDBm = par("csThreshDBm");
    turnAroundTime = par("turnAroundTime");
    acknowledgementSize = par("acknowledgementSize");

    // Extract simulation gates from NED file.
    fromMACGateId = findGate("fromMAC");
    assert(fromMACGateId != -1);
    toMACGateId = findGate("toMAC");
    assert(toMACGateId != -1);
    fromChannelGateId = findGate("fromChannel");
    assert(fromChannelGateId != -1);
    toChannelGateId = findGate("toChannel");
    assert(toChannelGateId != -1);

    // Initialise transceiver state.
    transceiverState = TXRX_STATE_RECEIVE;
    currentSignalPower = 0.0;

    csTimeMessage = new omnetpp::cMessage("csTimeMessage");
    turnAroundToTransmitCompleted = new omnetpp::cMessage("turnAroundToTransmitCompleted");
    turnAroundToReceiveCompleted = new omnetpp::cMessage("turnAroundToReceiveCompleted");
    endOfTransmissionMessage = new omnetpp::cMessage("endOfTransmissionMessage");
}

void Transceiver::finish() {
    cancelAndDelete(csTimeMessage);
    cancelAndDelete(turnAroundToTransmitCompleted);
    cancelAndDelete(turnAroundToReceiveCompleted);
    cancelAndDelete(endOfTransmissionMessage);

    // Dump this to get some more final info.
    EV << HERE << "currentTransmissions.size() = " << currentTransmissions.size() << std::endl;

    currentTransmissions.clear();

    if (currentTransmissionMacMessage != nullptr) {
        delete currentTransmissionMacMessage;
    }
}

void Transceiver::handleSelfMessage(omnetpp::cMessage *msg) {
    if (msg == csTimeMessage) {
        EV << HERE << "INFO: received self-message csTimeMessage." << std::endl;
        processCarrierSense();
        return;
    }

    if (msg == turnAroundToTransmitCompleted) {
        EV << HERE << "INFO: received self-message turnAroundToTransmitCompleted." << std::endl;
        processTurnAroundToTransmitCompleted();
        return;
    }

    if (msg == turnAroundToReceiveCompleted) {
        EV << HERE << "INFO: received self-message turnAroundToReceiveCompleted." << std::endl;
        processTurnAroundToReceiveCompleted();
        return;
    }

    if (msg == endOfTransmissionMessage) {
        EV << HERE << "INFO: received self-message endOfTransmissionMessage." << std::endl;
        processEndOfTransmission();
        return;
    }

    EV << HERE << "ERROR: self-message is of unexpected type." << std::endl;
    error("Shouldn't get here");
    delete msg; // prevent memory leaks
}

void Transceiver::handleMessage(omnetpp::cMessage *msg) {
    if (msg->isSelfMessage()) {
        return handleSelfMessage(msg);
    }

    tryHandleMessage(msg, SignalStart*, fromChannelGateId, handleSignalStart);
    tryHandleMessage(msg, SignalStop*, fromChannelGateId, handleSignalStop);
    tryHandleMessage(msg, CSRequest*, fromMACGateId, handleCSRequest);
    tryHandleMessageAnyGate(msg, TransmissionRequest*, handleTransmissionRequest);

    // Unexpected message if we get to this point.
    error("ERROR: msg is an unexpected type.");
}

void Transceiver::handleSignalStart(SignalStart *msg) {
    if (transceiverState != TXRX_STATE_RECEIVE) {
        EV << HERE << "WARNING: Received SignalStart when transceiver isn't in receive state. Moving on." << std::endl;
        delete msg; // prevent memory leaks
        return;
    }

    EV << HERE << "INFO: handling an incoming SignalStart packet" << std::endl;

    // Check if signal of the same ID is currently transmitting.
    if (currentTransmissions.find(msg->getIdentifier()) != currentTransmissions.end()) {
        EV << HERE << "ERROR: Detected new signal transmission for an already transmitting Node." << std::endl;
        error("Aborted, check logs.");
    }

    // Check for collisions on the channel.
    if (currentTransmissions.size() > 0) {
        EV << HERE << "Marked SignalStart message with id " << msg->getIdentifier() << " as collided" << std::endl;
        msg->setCollidedFlag(true);
        markAllMessagesAsCollided();
    }

    // Add signal to the transmissions map.
    EV << HERE << "Mapping " << msg->getIdentifier() << " to " << msg << std::endl;
    currentTransmissions.emplace(msg->getIdentifier(), msg);
}

SignalStart *Transceiver::getTransmittingSignalById(int signalId, bool andRemove) {
    if (currentTransmissions.count(signalId) < 1) {
        return nullptr;
    }

    SignalStart *storedSignal = (SignalStart *) currentTransmissions[signalId];
    assert(storedSignal != nullptr);

    if (andRemove) {
        EV << HERE << "Removing SignalStart message with id " << signalId << std::endl;
        currentTransmissions.erase(currentTransmissions.find(signalId));
    }

    return storedSignal;
}

void Transceiver::markAllMessagesAsCollided() {
    for (std::map<int, SignalStart *>::iterator iter = currentTransmissions.begin();
        iter != currentTransmissions.end(); ++iter) {
        iter->second->setCollidedFlag(true);
    }
}

void Transceiver::handleSignalStop(SignalStop *msg) {
    if (transceiverState != TXRX_STATE_RECEIVE) {
        EV << HERE << "WARNING: Received SignalStop when transceiver isn't in receive state. Moving on..." << std::endl;
        delete msg; // prevent memory leaks
        return;
    }

    EV << HERE << "INFO: handling an incoming SignalStop packet with id " << msg->getIdentifier() << std::endl;
    SignalStart *storedSignalStart = getTransmittingSignalById(msg->getIdentifier(), true);

    if (storedSignalStart == nullptr) {
        EV << HERE << "ERROR: Attempted to stop a signal that wasn't being transmitted from any Node." << std::endl;
        error("Aborted, check logs.");
    }

    // If there is a collision, simply remove it from the current transmissions and move on.
    // If there is NOT a collision, continue with extra processing.
    if (storedSignalStart->getCollidedFlag()) {
        EV << HERE << "INFO: Detected a collision, moving on." << std::endl;
    } else {
        EV << HERE << "INFO: No collisions present, processing..." << std::endl;

        MacMessage *storedMacMessage = (MacMessage *) storedSignalStart->decapsulate();
        assert(storedMacMessage != nullptr);

        double distance = MathHelpers::calculateEuclideanDistance(storedSignalStart->getPositionX(),
                                                              storedSignalStart->getPositionY(),
                                                              transceiverPositionX,
                                                              transceiverPositionY);
        double bitErrorRate = calculateBitErrorRate(distance, storedSignalStart->getTransmitPowerDBm());

        int packetLengthBits = storedMacMessage->getPacketLengthBits();
        double packetErrorProbability = MathHelpers::calculatePacketErrorProbability(packetLengthBits, bitErrorRate);

        // If this check fails, halt any further processing.
        double uniformDistribution01 = uniform(0, 1);
        if ((1 - packetErrorProbability) >= uniformDistribution01) {
            EV << HERE << "INFO: Packet is NOT erroneous (passed the random check), processing..." << std::endl;

            // Use previously extracted MacMessage.
            // If this check fails, halt any further processing (address filtering).
            if (storedMacMessage->getReceiverNodeId() == parentNodeId) {
                EV << HERE << "INFO: MacMessage's destination is this node, processing..." << std::endl;

                // Encapsulate into a message of type TransmissionIndication and send to local MAC.
                omnetpp::cPacket *transmissionIndication = new TransmissionIndication();
                transmissionIndication->encapsulate(storedMacMessage);
                send(transmissionIndication, toMACGateId);

                if (storedMacMessage->getType() != MAC_PACKET_TYPE_ACK) {
                    // Create MAC acknowledgment packet.
                    MacMessage *macAck = new MacMessage();
                    macAck->setReceiverNodeId(storedMacMessage->getTransmitterNodeId());
                    macAck->setTransmitterNodeId(parentNodeId);
                    macAck->setPacketLengthBits(acknowledgementSize);
                    macAck->setType(MAC_PACKET_TYPE_ACK);

                    // Send self message containing MAC acknowledgment packet wrapped in a
                    // TransmissionRequest packet to kick off transmission.
                    TransmissionRequest *transmissionRequest = new TransmissionRequest();
                    transmissionRequest->encapsulate(macAck);
                    handleTransmissionRequest(transmissionRequest);
                }

            } else {
                EV << HERE << "INFO: MacMessage's destination is NOT this node, moving on." << std::endl;
                delete storedMacMessage;
            }

        } else {
            EV << HERE << "INFO: Packet is erroneous (failed the random check), moving on." << std::endl;
            delete storedMacMessage;
        }
    }

    delete msg;
    delete storedSignalStart;
}

double Transceiver::calculateBitErrorRate(double distance, double signalTxPowerDBm) {
    double pathLossDB = MathHelpers::normalToDecibels(MathHelpers::calculatePathLoss(distance, pathLossExponent));
    double receivedPowerDBm = MathHelpers::calculateReceivedPower(signalTxPowerDBm, pathLossDB);
    double SNRRelation = MathHelpers::calculateSNRRelation(receivedPowerDBm, noisePowerDBm, bitRate);
    double bitErrorRate = MathHelpers::calculateBitError(SNRRelation);

    EV << HERE << "INFO: BER Calculations --> "
       << "  pathLossDB = " << pathLossDB
       << ", receivedPowerDBm = " << receivedPowerDBm
       << ", SNRRelation = " << SNRRelation
       << ", bitErrorRate = " << bitErrorRate
       << ", currSimTime = " << omnetpp::simTime()
       << std::endl;

    return bitErrorRate;
}

void Transceiver::handleCSRequest(CSRequest *msg) {
    EV << HERE << "INFO: handling an incoming CSRequest packet." << std::endl;

    if (transceiverState == TXRX_STATE_RECEIVE) {
        // Re-calculate observed signal power on the channel.
        currentSignalPower = calculateCurrentSignalPower();

        // Wait for csTime parameter.
        EV << HERE <<"The time is now " << omnetpp::simTime() << " and csTimeMessage is scheduled for " << csTime << " seconds time." << std::endl;
        scheduleAt(omnetpp::simTime() + csTime, csTimeMessage);
    } else if (transceiverState == TXRX_STATE_TRANSMIT || transceiverState == TXRX_STATE_TURNING_AROUND) {
        EV << HERE << "WARNING: Received CSRequest when transceiver is in transmit or turning around state!" << std::endl;

        // Handle the edge case when the transceiver receives a carrier sense request, and it is busy.
        CSResponse *csResponse = new CSResponse();
        csResponse->setBusyChannel(true);
        send(csResponse, toMACGateId);
    }

    delete msg;
}

void Transceiver::processCarrierSense() {
    bool busyChannel = (currentSignalPower > csThreshDBm);

    // Create CSResponse packet and send to local MAC.
    CSResponse *csResponse = new CSResponse();
    csResponse->setBusyChannel(busyChannel);
    send(csResponse, toMACGateId);
}

double Transceiver::calculateCurrentSignalPower() {
    double currentSignalPower = 0.0;
    for (std::map<int, SignalStart *>::iterator iter = currentTransmissions.begin();
         iter != currentTransmissions.end(); ++iter) {
        SignalStart *storedSignal = (SignalStart *) iter->second;
        currentSignalPower += MathHelpers::decibelsToNormal(storedSignal->getTransmitPowerDBm());
    }
    return MathHelpers::normalToDecibels(currentSignalPower);
}

void Transceiver::handleTransmissionRequest(TransmissionRequest *msg) {
    if (transceiverState == TXRX_STATE_RECEIVE) {
        MacMessage *storedMacMessage = (MacMessage *) msg->decapsulate();
        assert(storedMacMessage != nullptr);

        assert(currentTransmissionMacMessage == nullptr);
        currentTransmissionMacMessage = storedMacMessage;

        // Wait for turnAroundTime parameter.
        transceiverState = TXRX_STATE_TURNING_AROUND;
        EV << HERE << "INFO: Starting turn around from receive to transmit state..." << std::endl;
        scheduleAt(omnetpp::simTime() + turnAroundTime, turnAroundToTransmitCompleted);

    } else if (transceiverState == TXRX_STATE_TRANSMIT || transceiverState == TXRX_STATE_TURNING_AROUND) {

        if (transceiverState == TXRX_STATE_TRANSMIT) {
            EV << HERE
               << "WARNING: Received TransmissionRequest while transceiver state is transmit. Possibly a flawed MAC implementation?"
               << std::endl;
        } else if (transceiverState == TXRX_STATE_TURNING_AROUND) {
            EV << HERE << "WARNING: Received TransmissionRequest while transceiver state is TURNING_AROUND!!" << std::endl;
        }

        // Create a busy TransmissionConfirm packet and send to local MAC.
        TransmissionConfirm *transmissionConfirm = new TransmissionConfirm();
        transmissionConfirm->setStatus(statusBusy);
        send(transmissionConfirm, toMACGateId);
    }

    delete msg;
}

void Transceiver::processStartOfTransmission() {
    if (currentTransmissionMacMessage == nullptr) {
        EV << HERE
           << "ERROR: Trying to process the transmission request but the currentTransmissionMacMessage was null!"
           << std::endl;
        error("Aborted, check error logs!");
    }

    double packetLengthBits = currentTransmissionMacMessage->getBitLength();

    // Create SignalStart packet and send to channel.
    SignalStart *signalStart = new SignalStart();
    signalStart->setTransmitPowerDBm(txPowerDBm);
    signalStart->setPositionX(transceiverPositionX);
    signalStart->setPositionY(transceiverPositionY);
    signalStart->setIdentifier(parentNodeId);
    signalStart->setCollidedFlag(false);
    signalStart->encapsulate(currentTransmissionMacMessage);
    send(signalStart, toChannelGateId);

    // Reset currentTransmissionMacMessage and schedule the sending of the endOfTransmissionMessage.
    currentTransmissionMacMessage = nullptr;
    scheduleAt(omnetpp::simTime() + (packetLengthBits / bitRate), endOfTransmissionMessage);
}

void Transceiver::processEndOfTransmission() {
    // Create SignalStop packet and send to channel.
    SignalStop *signalStop = new SignalStop();
    signalStop->setIdentifier(parentNodeId);
    send(signalStop, toChannelGateId);

    // Wait for turnAroundTime parameter.
    transceiverState = TXRX_STATE_TURNING_AROUND;
    EV << HERE << "INFO: Starting turn around from transmit to receive state..." << std::endl;
    scheduleAt(omnetpp::simTime() + turnAroundTime, turnAroundToReceiveCompleted);
}

void Transceiver::processTurnAroundToTransmitCompleted() {
    EV << HERE << "INFO: Successfully entered transmit state." << std::endl;
    transceiverState = TXRX_STATE_TRANSMIT;
    processStartOfTransmission();
}

void Transceiver::processTurnAroundToReceiveCompleted() {
    EV << HERE << "INFO: Successfully entered receive state." << std::endl;
    transceiverState = TXRX_STATE_RECEIVE;
    TransmissionConfirm *transmissionConfirm = new TransmissionConfirm();
    transmissionConfirm->setStatus(statusOk);
    send(transmissionConfirm, toMACGateId);
}
