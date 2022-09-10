package mac.modules.transceiver;

simple Transceiver
{
	parameters:
	    // The id of this transceiver node.
	    int nodeId;
         
     	// Specifies the radiated power in dBm.
		double txPowerDBm @unit(dBm);
         
         // Specifies the transmission bitrate in bits/s.
		double bitRate @unit(bps);
		
		// Specifies the observed signal power (in dBm) above which a carrier-sensing station will indicate
		// the medium as busy.
		double csThreshDBm @unit(dBm);		
		
		// Specifies the noise power.
		double noisePowerDBm @unit(dBm);
		
		// Specifies the path loss exponent for the log-distance path loss model implemented in the Transceiver.
		// The Transceiver is not the most natural place for this parameter, but it is to be used here.
        double pathLossExponent;
         
        // Specifies the time required for turning the transceiver from the transmit state to the receive state
        // or vice versa. The transceiver is deaf during this turnaround.
        double turnAroundTime @unit(s);
        
        // Specifies the time required to carry out a carrier sense operation.
		double csTime @unit(s);
		
		// The nodeXPosition and nodeYPosition parameters describe the geographical position of the node in
		// two-dimensional space. Putting these parameters into the Transceiver is not really natural, but the
		// Transceiver is the only module really using the node positions (for bit error rate calculations).
		double nodeXPosition;
		double nodeYPosition;
		
		// This gives the size of a MAC acknowledgement packet as a number of bits.
		int acknowledgementSize @unit(b);
		
    gates:
        // The gates to and from the MAC module.
        input fromMAC;
        output toMAC;

        // The gates to and from the Channel module.
        input fromChannel;
        output toChannel;        
}
