package csma_3_nodes;

import mac.modules.packetgenerator.PacketGenerator;
import mac.modules.packetsink.PacketSink;
import mac.modules.mediumaccesscontrol.MediumAccessControl;
import mac.modules.transceiver.Transceiver;
import mac.modules.channel_.Channel;

network csma_3_nodes
{
    @display("bgb=565,575");
    submodules:
        tx_packetgenerator1: PacketGenerator {
            @display("p=80,59");
        }
        tx_mac1: MediumAccessControl {
            @display("p=80,162");
        }
        tx_txrx1: Transceiver {
            @display("p=80,274");
        }

        tx_packetgenerator2: PacketGenerator {
            @display("p=244,59");
        }
        tx_mac2: MediumAccessControl {
            @display("p=244,162");
        }
        tx_txrx2: Transceiver {
            @display("p=244,274");
        }

        tx_mac3: MediumAccessControl {
            @display("p=425,162");
        }
        tx_packetgenerator3: PacketGenerator {
            @display("p=425,59");
        }
        tx_txrx3: Transceiver {
            @display("p=425,274");
        }

        channel_: Channel {
            @display("p=244,398");
            gates:
                fromTransmitters[3];
                toTransmitters[3];
        }

        rx_packetsink: PacketSink {
            @display("p=439,491");
        }
        rx_mac: MediumAccessControl {
            @display("p=342,491");
        }
        rx_txrx: Transceiver {
            @display("p=244,491");
        }
    connections:
        tx_packetgenerator1.toMac --> tx_mac1.fromHigherLayer;
        tx_packetgenerator1.fromMac <-- tx_mac1.toHigherLayer;
        tx_mac1.toTransceiver --> tx_txrx1.fromMAC;
        tx_mac1.fromTransceiver <-- tx_txrx1.toMAC;
        tx_txrx1.toChannel --> channel_.fromTransmitters[0];
        tx_txrx1.fromChannel <-- channel_.toTransmitters[0];

        tx_packetgenerator2.toMac --> tx_mac2.fromHigherLayer;
        tx_packetgenerator2.fromMac <-- tx_mac2.toHigherLayer;
        tx_mac2.toTransceiver --> tx_txrx2.fromMAC;
        tx_mac2.fromTransceiver <-- tx_txrx2.toMAC;
        tx_txrx2.toChannel --> channel_.fromTransmitters[1];
        tx_txrx2.fromChannel <-- channel_.toTransmitters[1];

        tx_packetgenerator3.toMac --> tx_mac3.fromHigherLayer;
        tx_packetgenerator3.fromMac <-- tx_mac3.toHigherLayer;
        tx_mac3.toTransceiver --> tx_txrx3.fromMAC;
        tx_mac3.fromTransceiver <-- tx_txrx3.toMAC;
        tx_txrx3.toChannel --> channel_.fromTransmitters[2];
        tx_txrx3.fromChannel <-- channel_.toTransmitters[2];

        rx_packetsink.toMAC --> rx_mac.fromHigherLayer;
        rx_packetsink.fromMAC <-- rx_mac.toHigherLayer;
        rx_mac.toTransceiver --> rx_txrx.fromMAC;
        rx_mac.fromTransceiver <-- rx_txrx.toMAC;
        rx_txrx.toChannel --> channel_.fromReceiver;
        rx_txrx.fromChannel <-- channel_.toReceiver;
}
