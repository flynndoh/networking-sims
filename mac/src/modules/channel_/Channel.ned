package mac.modules.channel_;

simple Channel
{
    gates:
        // The gates to and from the single Receiver module.
        input fromReceiver;
        output toReceiver;

        // The gates to and from the collection of Transceiver modules.
        input fromTransmitters[];
		output toTransmitters[];
}
