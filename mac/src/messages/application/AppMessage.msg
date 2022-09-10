packet AppMessage {
    
    // Simulation time at which the message has been generated.
    simtime_t timeStamp;
    
    // The node identification of the TransmitterNode sending this message this has to be obtained from the
    // parent module (which should be a TransmitterNode compound module).
    int senderId;
    
    // By convention, the receiver will have an ID of 0 but it could be useful to specify this in the INI file.
    int receiverId;
    
    // The current value of the seqno variable is copied into this field and afterwards seqno is incremented.
    int sequenceNumber;
    
    // The total message size in bytes, to be taken from the msgSizeDistribution module parameter.
    int msgSize;
    
}
