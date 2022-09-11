package wlan.simulations;

import wlan.modules.transmitter.Transmitter;
import wlan.modules.receiver.Receiver;
import wlan.modules._channel.Channel;

network wlan_sim
{
    @display("bgb=283,293");
    submodules:
        tx: Transmitter {
            @display("p=44.006664,49.746666");
        }
        rx: Receiver {
            @display("p=198,224");
        }
        ch: Channel {
            @display("p=118,130");
        }
    connections:
        tx.toChannel --> ch.fromTransmitter;
        ch.toTransmitter --> tx.fromChannel;
        ch.toReceiver --> rx.fromChannel;
}
