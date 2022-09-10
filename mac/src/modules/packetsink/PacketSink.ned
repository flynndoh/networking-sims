package mac.modules.packetsink;

simple PacketSink
{
	parameters:
        @signal[delay](type=simtime_t);
		@signal[gap](type=long);
        @statistic[delay](record=stats);
        @statistic[gap](record=stats);
        
    gates:
        // The gates to and from the MAC module.
    	input fromMAC;    	
    	output toMAC;    	
}
