#include "Channel.h"

Define_Module(Channel);

enum channelStates {
    GOOD = 0,
    BAD = 1
} nextBitChannelState;

void Channel::initialize () {
    // Extract simulation parameters from NED file.
    bitRate = par("bitRate");
    pathLossExponent = par("pathLossExponent");
    nodeDistance = par("nodeDistance");
    txPowerDBm = par("txPowerDBm");
    noisePowerDBm = par("noisePowerDBm");
    channelGainGoodDB = par("channelGainGoodDB");
    channelGainBadDB = par("channelGainBadDB");
    txProbabilityGoodGood = par("txProbabilityGoodGood");
    txProbabilityBadBad = par("txProbabilityBadBad");

    // Extract simulation gates from NED file.
    fromTransmitterGateId = findGate("fromTransmitter");
    toTransmitterGateId = findGate("toTransmitter");
    toReceiverGateId = findGate("toReceiver");

    completion = new PacketCompletionMessage();
    currentPacket = nullptr;
    nextBitChannelState = GOOD;

    // Send initial packet request message to kick things off.
    send(new RequestPacketMessage(), toTransmitterGateId);
}

void Channel::finish() {
    cancelAndDelete(completion);
}

void Channel::handleMessage(omnetpp::cMessage* msg) {
    if (msg == completion) {
        EV << HERE << "handling completion event" << std::endl;
        handleCompletion();
    } else if ((dynamic_cast<ResponsePacket*>(msg)) && (msg->arrivedOn(fromTransmitterGateId))) {
        EV << HERE << "handling an incoming packet" << std::endl;
        handleIncomingPacket((ResponsePacket*) msg);
    } else {
        error("Channel::handleMessage: received unforeseen message!");
    }
}

void Channel::handleIncomingPacket(ResponsePacket* pkt) {
    // Is the channel currently busy with another packet?
    if (currentPacket != nullptr) {
        EV << HERE << "got packet while channel is busy, dropping it" << std::endl;
        delete pkt;
        return;
    }

    currentPacket = pkt;
    int packetLengthBits = pkt->getOverheadBits() + pkt->getUserBits();
    int packetBitErrorsSoFar = 0;

    double channelStateUniformDistribution01 = 0;
    double bitErrorRateUniformDistribution01 = 0;
    double bitErrorRate = 0;

    // Iterate through packet.
    for (int i = 0; i < packetLengthBits; i++) {
        channelStateUniformDistribution01 = uniform(0,1);

        // Determine channel state. Haven't condensed this logic for the sake of clarity.
        if (nextBitChannelState == GOOD) {
            if (channelStateUniformDistribution01 < txProbabilityGoodGood) {
                nextBitChannelState = GOOD;
            } else {
                nextBitChannelState = BAD;
            }
        } else {
            if (channelStateUniformDistribution01 < txProbabilityBadBad) {
                nextBitChannelState = BAD;
            } else {
                nextBitChannelState = GOOD;
            }
        }

        // Determine Bit Error Rate for current bit.
        bitErrorRate = calculateBitErrorRate();
        bitErrorRateUniformDistribution01 = uniform(0,1);
        if (bitErrorRateUniformDistribution01 < bitErrorRate) {
            // Mark packet as erroneous.
            pkt->setErrorFlag(true);
            packetBitErrorsSoFar = pkt->getBitErrorCount();
            pkt->setBitErrorCount(++packetBitErrorsSoFar);
        }
    }

    scheduleAt(omnetpp::simTime()+(packetLengthBits / bitRate), completion);
}

double Channel::calculateBitErrorRate() {
    double pathLossDB = MathHelpers::normalToDecibels(MathHelpers::calculatePathLoss(nodeDistance, pathLossExponent));
    double gain = determineGain(nextBitChannelState);
    double receivedPowerDBm = MathHelpers::calculateReceivedPower(txPowerDBm, pathLossDB, gain);
    double SNRRelation = MathHelpers::calculateSNRRelation(receivedPowerDBm, noisePowerDBm, bitRate);

    // Calculate bit error rate using the complementary error function.
    double bitErrorRate = erfc(sqrt(2 * MathHelpers::decibelsToNormal(SNRRelation)));

    EV << HERE << "BER Calculations -->"
           << "  pathLossDB = " << pathLossDB
           << ", gain = " << gain
           << ", receivedPowerDBm = " << receivedPowerDBm
           << ", SNRRelation = " << SNRRelation
           << ", bitErrorRate = " << bitErrorRate
           << ", currSimTime = " << omnetpp::simTime()
           << std::endl;

    return bitErrorRate;
}

double Channel::determineGain(bool channelState) {
    return (nextBitChannelState == GOOD) ? channelGainGoodDB : channelGainBadDB;
}

void Channel::handleCompletion() {
    if (currentPacket == nullptr) {
        error("Channel::handleCompletion: no packet to complete");
    }

    // Send current packet to receiver.
    EV << HERE << "Sending packet to receiver" << std::endl;
    send(currentPacket, toReceiverGateId);

    currentPacket = nullptr;

    // Ask transmitter for a new packet.
    EV << HERE << "Asking transmitter for new packet" << std::endl;
    send(new RequestPacketMessage(), toTransmitterGateId);
}
