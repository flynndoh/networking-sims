#include "PacketSink.h"

Define_Module(PacketSink);

omnetpp::simsignal_t delay = omnetpp::cComponent::registerSignal("delay");
omnetpp::simsignal_t gap = omnetpp::cComponent::registerSignal("gap");

void PacketSink::initialize() {
    // Extract simulation gates from NED file.
    fromMACGateId = findGate("fromMAC");
    assert(fromMACGateId != -1);
    toMACGateId = findGate("toMAC");
    assert(toMACGateId != -1);

    numReceived = 0;
}

void PacketSink::finish()
{
    transmitterLastSeqno.clear();
    recordScalar("numReceived", numReceived);
}

void PacketSink::handleMessage(omnetpp::cMessage *msg) {
    tryHandleMessage(msg, AppMessage*, fromMACGateId, handleAppMessage);
    error("ERROR: message is an unexpected type.");
}

void PacketSink::handleAppMessage(AppMessage *msg) {
    omnetpp::simtime_t now = omnetpp::simTime();
    bool isSequenceNumberValid = false;

    int senderId = msg->getSenderId();
    int currentSeqno = msg->getSequenceNumber();

    // Check to see if the senderId has been added to the map.
    if (transmitterLastSeqno.find(senderId) != transmitterLastSeqno.end()) {

        // Check that the received sequence number is strictly greater than the one we know about.
        if (currentSeqno > transmitterLastSeqno[senderId]) {

            // Update the valid sequence number.
            isSequenceNumberValid = true;
            transmitterLastSeqno[senderId] = msg->getSequenceNumber();

        } else {

            // We may have received a copy of a message we've already received.
            isSequenceNumberValid = false;

        }
    } else {
        // Map the senderId to the valid sequence number.
        transmitterLastSeqno.emplace(senderId, msg->getSequenceNumber());
        isSequenceNumberValid = true;
    }

    if (isSequenceNumberValid) {

        numReceived++;

        EV << HERE << "received valid message" << ", timeStamp = "
                  << msg->getTimeStamp() << ", senderId = "
                  << msg->getSenderId() << ", sequenceNumber = "
                  << msg->getSequenceNumber() << ", msgSize = "
                  << msg->getMsgSize() << std::endl;

        // Emit statistic for delay.
        emit(delay, now - msg->getTimeStamp());

        // Transmitter has already been registered, calculate and emit statistic for gap.
        emit(gap, currentSeqno - transmitterLastSeqno.find(senderId)->second);

    } else {
        EV << HERE << "received the same message again!" << std::endl;
    }

    delete msg;
}
