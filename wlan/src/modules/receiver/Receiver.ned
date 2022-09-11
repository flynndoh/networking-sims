package wlan.modules.receiver;

simple Receiver {
    parameters:
        @signal[errorRate](type=long);
        @statistic[errorRate](record=stats);
    gates:
    	input fromChannel;
}