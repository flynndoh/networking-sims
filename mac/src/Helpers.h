#ifndef HELPERS_H_
#define HELPERS_H_

class Helpers {
  public:
    static double calculateEuclideanDistance(double positionX1, double positionY1, double positionX2, double positionY2);
    static double calculatePathLoss(double distanceMeters, double pathLossExp);
    static double calculateReceivedPower(double txPowerDBm, double pathLossDB);
    static double calculateSNRRelation(double receivedPowerDBm, double noisePowerDBm, double bitRate);
    static double normalToDecibels(double target);
    static double decibelsToNormal(double target);
    static double calculateBitError(double SNRRelation);
    static double calculatePacketErrorProbability(double packetLengthBits, double bitErrorRate);
};

#endif /* HELPERS_H_ */
