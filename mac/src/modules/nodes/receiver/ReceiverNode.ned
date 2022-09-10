package mac.modules.nodes.receiver;

import mac.modules.packetsink.PacketSink;
import mac.modules.mediumaccesscontrol.MediumAccessControl;
import mac.modules.transceiver.Transceiver;

module ReceiverNode
{
    parameters:
        // An integer value used to identify the individual node.
        int nodeIdentifier;

        @display("bgb=400,272");
    gates:
        input fromChannel;
        output toChannel;

    submodules:
        packetsink: PacketSink {
            @display("p=243,48");
        }
        mac: MediumAccessControl {
            @display("p=197,190");
        }
        transceiver: Transceiver {
            @display("p=134,48");
            nodeId = nodeIdentifier;
        }
    connections:
        transceiver.toChannel --> toChannel;
        transceiver.fromChannel <-- fromChannel;
		transceiver.toMAC --> mac.fromTransceiver;
        transceiver.fromMAC <-- mac.toTransceiver;
        mac.toHigherLayer --> packetsink.fromMAC;
        mac.fromHigherLayer <-- packetsink.toMAC;
}
