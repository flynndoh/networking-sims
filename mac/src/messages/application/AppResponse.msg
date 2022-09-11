packet AppResponse {
    
    // The corresponding AppMessage's sequenceNumber that the AppResponse refers to.
    int sequenceNumber;
    
   	// The outcome for that AppMessage which is either success (if the 
   	// underlying MAC has received an acknowledgement from the receiver), 
   	// a drop (if the MAC has dropped the message at the buffer) or a failure
   	// (when the MAC did not manage to receive an acknowledgement for the 
   	// MAC packet containing the message).
    bool success;

}