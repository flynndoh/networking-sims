#include "Transmitter.h"

Define_Module(Transmitter);

void Transmitter::initialize() {
    // Extract simulation parameters from NED file.
    overheadBits = par("numberOverheadBits");
    userBits = par("numberUserBits");

    // Extract simulation gates from NED file.
    toChannelGateId = findGate("toChannel");
    fromChannelGateId = findGate("fromChannel");
}

void Transmitter::handleMessage(omnetpp::cMessage* msg) {
    if ((dynamic_cast<RequestPacketMessage*>(msg)) && (msg->arrivedOn(fromChannelGateId))) {
        EV << HERE << "generating a new packet at time "
           << omnetpp::simTime() << " with sequence number " << sequenceNumber
           << " and with packet size " << (overheadBits + userBits)
           << " bits" << std::endl;

        delete msg;

        ResponsePacket* pkt = createResponsePacket();
        send(pkt, toChannelGateId);
    } else {
        error("Transmitter::handleMessage: received unforeseen message!");
    }
}

ResponsePacket* Transmitter::createResponsePacket() {
    ResponsePacket* pkt = new ResponsePacket;
    pkt->setSequenceNumber(sequenceNumber++);
    pkt->setOverheadBits(overheadBits);
    pkt->setUserBits(userBits);
    pkt->setBitError(false);
    return pkt;
}

