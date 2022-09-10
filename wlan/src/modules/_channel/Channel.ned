package wlan_simulation.modules._channel;

simple Channel {
    parameters:
        
        // Specifies the distance between transmitter and receiver nodes in meters.
        double nodeDistance @unit(m);
        
        // The path loss exponent to be used in the log-distance channel model.
        double pathLossExponent @unit(dBm);
        
        // Specifies the radiated power in dBm.
		double txPowerDBm @unit(dBm);
        
        // Specifies the transmission bitrate in bits/s.
		double bitRate @unit(bps);
        
        // Specifies the noise power (which here we purport to be expressed in units of dBm, which is not entirely correct).
		double noisePowerDBm @unit(dBm);
        
        // If we are in the Good state during bit n, then with probability txProbabilityGoodGood we will also be in the Good state
        // during bit n + 1. With the converse probability 1 − txProbabilityGoodGood the channel will be in the Bad state during
        // bit n + 1.
		double txProbabilityGoodGood;
        
        // Transition probability of going from the Bad state during bit n to the Bad state for bit n + 1.
		double txProbabilityBadBad;
        
        // Channel gain (in dB) during the Good state.
		double channelGainGoodDB @unit(dB);
        
        // Channel gain (in dB) during the Bad state.
		double channelGainBadDB @unit(dB);
        
    gates:
    	input fromTransmitter;
    	output toTransmitter;
    	output toReceiver;
}
