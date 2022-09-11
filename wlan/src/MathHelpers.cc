#include "MathHelpers.h"

double MathHelpers::calculateEuclideanDistance(double positionX1, double positionY1, double positionX2, double positionY2) {
    return sqrt(pow((positionX1 - positionX2), 2) + pow((positionY1 - positionY2), 2));
}

double MathHelpers::calculatePathLoss(double distanceMeters, double pathLossExp) {
    return (distanceMeters > 1) ? pow(distanceMeters, pathLossExp) : 1;
}

double MathHelpers::calculateReceivedPower(double txPowerDBm, double pathLossDB) {
    return txPowerDBm - pathLossDB;
}

double MathHelpers::calculateReceivedPower(double txPowerDBm, double pathLossDB, double gain) {
    return calculateReceivedPower(txPowerDBm, pathLossDB) + gain;
}

double MathHelpers::calculateSNRRelation(double receivedPowerDBm, double noisePowerDBm, double bitRate) {
    return receivedPowerDBm - (noisePowerDBm + normalToDecibels(bitRate));
}

double MathHelpers::normalToDecibels(double target) {
    return 10 * log10(target);
}

double MathHelpers::decibelsToNormal(double target) {
    return pow(10, (target / 10));
}

double MathHelpers::calculateBitError(double SNRRelation) {
    // Calculate bit error rate using the complementary error function.
    return erfc(sqrt(2 * decibelsToNormal(SNRRelation)));
}

double MathHelpers::calculatePacketErrorProbability(double packetLengthBits, double bitErrorRate) {
    return bitErrorRate * packetLengthBits;
}
