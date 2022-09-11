#include "Receiver.h"

Define_Module(Receiver);

omnetpp::simsignal_t errorRateSId = omnetpp::cComponent::registerSignal("errorRate");

void Receiver::initialize () {
    // Extract simulation gates from NED file.
    inGateId = findGate("fromChannel");
}

void Receiver::handleMessage(omnetpp::cMessage* msg) {
    if (dynamic_cast<ResponsePacket*>(msg) && msg->arrivedOn(inGateId)) {
        ResponsePacket* pkt = (ResponsePacket*) msg;

        EV << "Receiver::handleMessage: got valid message"
        << ", sequenceNumber = " << pkt->getSequenceNumber()
        << ", length = " << pkt->getOverheadBits() + pkt->getUserBits()
        << ", overheadBits = " << pkt->getOverheadBits()
        << ", userBits = " << pkt->getUserBits()
        << ", errorFlag = " << pkt->getErrorFlag()
        << ", currSimTime = " << omnetpp::simTime()
        << std::endl;

        emit(errorRateSId, pkt->getErrorFlag());
        delete msg;

    } else {
        error("Receiver::handleMessage: received unforeseen message!");
    }
}
