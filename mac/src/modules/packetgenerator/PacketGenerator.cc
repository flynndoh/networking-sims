#include "PacketGenerator.h"

Define_Module(PacketGenerator);

void PacketGenerator::initialize() {
    // Extract simulation parameters from NED file.
    senderId = par("senderId");
    receiverId = par("receiverId");

    // Extract simulation gates from NED file.
    toMacGateId = findGate("toMac");
    assert(toMacGateId != -1);
    fromMacGateId = findGate("fromMac");
    assert(fromMacGateId != -1);

    numSent = 0;

    // Used to trigger the sending of an AppMessage.
    readyMessage = new omnetpp::cMessage("ready");
    assert(readyMessage != nullptr);

    // Send a self-message to trigger the start of the packet generation.
    scheduleAt(omnetpp::simTime(), readyMessage);
}

void PacketGenerator::finish()
{
    recordScalar("numSent", numSent);
}

double PacketGenerator::getIatDistribution(void) {
    return par("iatDistribution");
}

double PacketGenerator::getMsgSizeDistribution(void) {
    return par("msgSizeDistribution");
}

void PacketGenerator::handleMessage(omnetpp::cMessage *msg) {
    // Check if the message is telling us to send an AppMessage.
    if (msg->isSelfMessage()) {
        if (msg == readyMessage) {
            EV << HERE
                      << "INFO: received readyMessage, sending AppMessage to MAC layer."
                      << std::endl;
            sendAppMessage();
            return;
        }

        EV << HERE << "CRITICAL: Received an unexpected self-message" << std::endl;
        error("Received an unexpected self-message");
        delete msg;
        return;
    }

    // Check that the message is an AppReport.
    tryHandleMessage(msg, AppResponse*, fromMacGateId, handleAppResponse);

    EV << HERE << "ERROR: msg is not an AppResponse." << std::endl;
    error("Should not get here");
    delete msg; // Prevent memory leaks.
}

void PacketGenerator::handleAppResponse(AppResponse *appResponse) {
    // The PacketGenerator just drops any AppReports it receives from the MAC layer.
    EV << HERE << "INFO: dropped AppResponse message." << std::endl;
    delete appResponse;
}

void PacketGenerator::sendAppMessage() {
    omnetpp::simtime_t now = omnetpp::simTime();

    // Create an AppMessage.
    AppMessage *appMessage = new AppMessage();
    appMessage->setMsgSize(getMsgSizeDistribution());
    appMessage->setSequenceNumber(seqNo++);
    appMessage->setTimeStamp(now);
    appMessage->setSenderId(senderId);
    appMessage->setReceiverId(receiverId);

    // Send the AppMessage to the MAC layer.
    send(appMessage, toMacGateId);
    numSent++;

    // Schedule another message.
    scheduleAt(now + getIatDistribution(), readyMessage);
}
