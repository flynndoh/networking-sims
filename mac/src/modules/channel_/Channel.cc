#include "../channel_/Channel.h"

#include "../../messages/signal/SignalStart_m.h"
#include "../../messages/signal/SignalStop_m.h"

#include "../../Utils.h"

Define_Module(Channel);

void Channel::initialize() {
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

void Channel::handleMessage(cMessage *msg) {
    // Attempt to handle the message as a SignalStart*.
    tryHandleMessageAnyGate(msg, SignalStart*, handleSignalMessage);

    // Attempt to handle the message as a SignalStop*.
    tryHandleMessageAnyGate(msg, SignalStop*, handleSignalMessage);

    EV << HERE << "ERROR: Received a message that we shouldn't have received!"
               << endl;
    error("Shouldn't get here");
    delete msg;
}

void Channel::handleSignalMessage(cMessage *msg) {
    // Deep copy the message and send to the transmitters.
    for (int i = 0; i < numTransmitters; i++) {
        cMessage *copy = msg->dup();
        send(copy, toTransmitterGateIds[i]);
    }

    // Send the original to the receiver.
    send(msg, toReceiverGateId);
}
