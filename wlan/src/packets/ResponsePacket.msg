packet ResponsePacket {
    
	// The sequence number set by the transmitter.	
	int sequenceNumber;
    
    // The number of overhead bits in a packet (header, trailer).
	int overheadBits;
	
	// The number of user data bits in a packet.
	int userBits;
	
	// Error flag that determines if the packet is erroneous.
	bool errorFlag;
	
	// The number of erroneous bits inside the packet.
	int bitErrorCount;
}
