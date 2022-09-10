#include "Helpers.h"
#include <cmath>

double Helpers::calculateEuclideanDistance(double positionX1, double positionY1, double positionX2, double positionY2)
{
    return sqrt(pow((positionX1 - positionX2), 2) + pow((positionY1 - positionY2), 2));
}

double Helpers::calculatePathLoss(double distanceMeters, double pathLossExp)
{
    return (distanceMeters > 1) ? pow(distanceMeters, pathLossExp) : 1;
}

double Helpers::calculateReceivedPower(double txPowerDBm, double pathLossDB)
{
    return txPowerDBm - pathLossDB;
}

double Helpers::calculateSNRRelation(double receivedPowerDBm, double noisePowerDBm, double bitRate)
{
    return receivedPowerDBm - (noisePowerDBm + normalToDecibels(bitRate));
}

double Helpers::normalToDecibels(double target)
{
    return 10 * log10(target);
}

double Helpers::decibelsToNormal(double target)
{
    return pow(10, (target / 10));
}

double Helpers::calculateBitError(double SNRRelation)
{
    // Calculate bit error rate using the complementary error function.
    return erfc(sqrt(2 * decibelsToNormal(SNRRelation)));
}

double Helpers::calculatePacketErrorProbability(double packetLengthBits, double bitErrorRate)
{
    return bitErrorRate * packetLengthBits;
}
