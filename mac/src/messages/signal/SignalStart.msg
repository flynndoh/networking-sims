packet SignalStart {
        
	// The copied value of the Transceiver's txPowerDBm parameter.
    double transmitPowerDBm;
    
	// The copied value of the Transceiver's nodeXPosition parameter.
    double positionX;
    
	// The copied value of the Transceiver's nodeYPosition parameter.
    double positionY;
    
    // Contains the value of the nodeIdentifier parameter of the surrounding compound module of type TransmitterNode
    // or ReceiverNode.
    int identifier;
    
    // Boolean flag which is initialised to FALSE.
    bool collidedFlag; 
}
