package mac.modules.nodes.transmitter;

import mac.modules.packetgenerator.PacketGenerator;
import mac.modules.mediumaccesscontrol.MediumAccessControl;
import mac.modules.transceiver.Transceiver;

module TransmitterNode {
    
	parameters:        
    	// An integer value used to identify the individual node.
    	int nodeIdentifier;
    	
    	// An integer value used to identify the receiver node.
	    int receiverId;
    	
    
	// Good resource here for multiple IO gates: 
    // https://stackoverflow.com/a/50155364/13326442
    gates:
    	input fromChannel;   
	   	output toChannel;    	
    	 	
    submodules:
        packetgenerator: PacketGenerator {
            parameters:
                senderId = nodeIdentifier;
                receiverId = receiverId;
        };
        mac: MediumAccessControl;
        transceiver: Transceiver {
            nodeId = nodeIdentifier;
        }
    
    connections:
        transceiver.toChannel --> toChannel;
        transceiver.fromChannel <-- fromChannel;
		transceiver.toMAC --> mac.fromTransceiver;
        transceiver.fromMAC <-- mac.toTransceiver;
        mac.toHigherLayer --> packetgenerator.fromMac;
        mac.fromHigherLayer <-- packetgenerator.toMac;
}
