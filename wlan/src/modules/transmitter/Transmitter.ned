package wlan.modules.transmitter;

simple Transmitter {
	parameters:
	    // The number of overhead bits in a packet (headers, trailers).
		int numberOverheadBits @unit(bit);
		
		// The user data size u in bits, which indicates the amount of user data contained in a packet.
		int numberUserBits @unit(bit);
			    
	gates:
		output toChannel;
		input fromChannel;		
}