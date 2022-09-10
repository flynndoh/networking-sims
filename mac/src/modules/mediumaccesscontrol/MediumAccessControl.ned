package mac.modules.mediumaccesscontrol;

simple MediumAccessControl
{
    parameters:        
		@signal[bufferDropped](type=bool);        
		@signal[lostInChannel](type=bool);        
        @statistic[bufferDropped](record=stats);
        @statistic[lostInChannel](record=stats);
        
        // The size (in packets) of a FIFO buffer for packets from higher layers.
        int bufferSize;
        
        // The max number of times a carrier sense operation may indicate a busy channel before declaring an
        // attempt failed.
        int maxBackoffs;
        
        // The random distribution for the waiting backoff time.
        volatile double backoffDistribution @unit(s);
        
        // After an attempt has failed, an additional waiting time following this distribution is inserted
        // before the next attempt is started.
        volatile double retransmissionDistribution @unit(s);
        
        // The amount of time to wait for timeout.
        double ackTimeout @unit(s) = default(5ms);
        
        // Specifies the maximum number of allowed attempts - must be at least one.
        int maxAttempts;
        
    gates:
        
        // The gates to and from the Transceiver module.
        input fromTransceiver;
        output toTransceiver;
        
        // The gates to and from the PacketSink/PacketGenerator module.
        input fromHigherLayer;
        output toHigherLayer;
}
