#include "Channel.h"

Define_Module(Channel);

void Channel::initialize() {
    // Extract simulation gates from NED file.
    fromReceiverGateId = findGate("fromReceiver");
    assert(fromReceiverGateId != -1);

    toReceiverGateId = findGate("toReceiver");
    assert(toReceiverGateId != -1);

    assert(gateSize("fromTransmitters") == gateSize("toTransmitters"));
    numTransmitters = gateSize("toTransmitters");

    fromTransmitterGateIds = (int*)calloc(numTransmitters, sizeof(int));
    assert(fromTransmitterGateIds != nullptr);

    toTransmitterGateIds = (int*)calloc(numTransmitters, sizeof(int));
    assert(toTransmitterGateIds != nullptr);

    for (int i = 0; i < numTransmitters; ++i) {
        fromTransmitterGateIds[i] = findGate("fromTransmitters", i);
        toTransmitterGateIds[i] = findGate("toTransmitters", i);
    }
}

void Channel::finish()
{
    free(fromTransmitterGateIds);
    fromTransmitterGateIds = nullptr;

    free(toTransmitterGateIds);
    toTransmitterGateIds = nullptr;
}

void Channel::handleMessage(omnetpp::cMessage *msg) {
    // Attempt to handle the message as a SignalStart*.
    tryHandleMessageAnyGate(msg, SignalStart*, handleSignalMessage);

    // Attempt to handle the message as a SignalStop*.
    tryHandleMessageAnyGate(msg, SignalStop*, handleSignalMessage);

    EV << HERE << "ERROR: Received a message that we shouldn't have received!" << std::endl;
    error("Channel::handleMessage: Shouldn't get here");
    delete msg;
}

void Channel::handleSignalMessage(omnetpp::cMessage *msg) {
    // Deep copy the message and send to the transmitters.
    for (int i = 0; i < numTransmitters; i++) {
        omnetpp::cMessage *copy = msg->dup();
        send(copy, toTransmitterGateIds[i]);
    }

    // Send the original to the receiver.
    send(msg, toReceiverGateId);
}
