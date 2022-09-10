enum MacPacketType {
    MAC_PACKET_TYPE_DATA = 0;
    MAC_PACKET_TYPE_ACK = 1;
}

packet MacMessage {
    // The packet's sequence number.
    int seqNo;

    // The original sequence number, used to determine which acknowledgement corresponds to which mac message.
    int originalSeqNo;
 	
 	// The node identifier of the receiver node.
   	int receiverNodeId;
    
	// The node identifier of the transmitter node.
   	int transmitterNodeId;
   	
   	// The total length of the packet (equal to the message size of the AppMessage).
   	int packetLengthBits;
   	
   	// The type of the MAC packet.
   	int type @enum(MacPacketType);   
}