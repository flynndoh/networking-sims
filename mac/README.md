[< back](../README.md)

---

# MAC Simulation
MAC layer simulation with transceiver carrier sensing using the [OMNeT++](https://omnetpp.org/) discrete event simulation library and framework.

## Fun Gif
![Gif of the simulation running](docs/img/mac.gif)

## Simulation

Below are the baseline settings for the MAC simulation. The scenario documented below adds to and overrides these variables to obtain interesting results. 
```
network = csma_3_nodes

sim-time-limit              = 1000s
simtime-resolution          = ps
**.*.iatDistribution        = 10ms
**.*.msgSizeDistribution    = 64B

**.*.txPowerDBm                  = 0dBm
**.*.bitRate                     = 250000bps
**.*.csThreshDBm                 = -50dBm
**.*.noisePowerDBm               = -120dBm
**.*.turnAroundTime              = 1ms
**.*.pathLossExponent            = 4
**.*.csTime                      = 125us
**.*.backoffDistribution         = exponential(3ms)
**.*.retransmissionDistribution  = exponential(10ms)
**.*.bufferSize                  = 5
**.*.maxBackoffs                 = 5
```

### CSMA with 3 Transceivers Scenario
The scenario's variables can be viewed and tweaked in the `simulations/csma_3_nodes/omnetpp.ini` file. If you would like to alter the topography of the network, you can do so by adding new nodes and connections in the `simulations/csma_3_nodes/csma_3_nodes.ned` file.

#### Variables
```
**.tx_packetgenerator1.receiverId   = 0
**.tx_packetgenerator1.senderId     = 1
**.tx_packetgenerator2.receiverId   = 0
**.tx_packetgenerator2.senderId     = 2
**.tx_packetgenerator3.receiverId   = 0
**.tx_packetgenerator3.senderId     = 3

**.*.maxAttempts            = 5
**.*.acknowledgementSize    = 100b

**.tx_txrx1.nodeId          = 1
**.tx_txrx1.nodeXPosition   = 10
**.tx_txrx1.nodeYPosition   = 10

**.tx_txrx2.nodeId          = 2
**.tx_txrx2.nodeXPosition   = 30
**.tx_txrx2.nodeYPosition   = 10

**.tx_txrx3.nodeId          = 3
**.tx_txrx3.nodeXPosition   = 50
**.tx_txrx3.nodeYPosition   = 10

**.rx_txrx.nodeId           = 0
**.rx_txrx.nodeXPosition    = 0
**.rx_txrx.nodeYPosition    = 0
```

### Modules

#### Packet Generator
The `PacketGenerator` generates `AppMessages` and sends them to the `MediumAccessControl` layer. To start the simulation, the `PacketGenerator` sends itself a self message, and when that message is received, the module will create an `AppMessage` and send it to the `MediumAccessControl` layer.

#### Medium Access Control (MAC)
- The `MediumAccessControl` module moderates how the `Transceiver` should communicate on the `Channel`.
- TODO: This deserves a much more detailed explanation.

#### Transceiver (Receiver + Transmitter)
- The `Transceiver` handles all sending and receiving from the `Channel`.
- The `Transceiver` cannot transmit and receive at the same time, and it is always in one of the following states: `TXRX_STATE_RECEIVE`, `TXRX_STATE_TRANSMIT` or `TXRX_STATE_TURNING_AROUND`
- TODO: This deserves a much more detailed explanation.

#### Channel
The `Channel` facilitates all messages from every transceiver to every other transceiver. It deals with transmission signals, namely `SignalStart` and `SignalStop` and keeps an index of all transmitters and their signals (if active).

#### Packet Sink
The `PacketSink` module is the final resting place for `AppMessages`. The `PacketSink` receives `AppMessages` from the `MediumAccessControl` layer and computes various statistics about the `AppMessage` it has received, such as tracking sequence number gaps and packet delay. 

### Messages/Packets

#### AppMessage
The `AppMessage` is meant to emulate a real (albeit simplified) packet that would have been sent from the application layer. 

#### AppResponse
The `AppResponse` is meant to emulate an acknowledgement that would be delivered to the application layer in a real system. In the simulation though the `PacketGenerator` simply discards this and moves on when the `MediumAccessControl` layer sends it.

#### CSRequest (Carrier Sense Request)
The `MediumAccessControl` layer sends `CSRequests` (carrier/channel sensing requests) to ask the `Transceiver` to "sense" or listen to the carrier/channel's current signal power. This information is sent using `CSResponses` and can be used by the `MediumAccessControl` layer to work out when it is okay to send `StartSignals` while avoiding collisions with other `StartSignals` that are being send by other `Transceivers`.

#### CSResponse (Carrier Sense Response)
The `Transceiver` sends `CSResponses` (carrier/channel sensing responses) to the `MediumAccessControl` layer to inform it if the `Channel` is currently occupied/busy. This is determined by the `Transceiver` listening to the current signal power on the `Channel` and if it exceeds some threshold, will report it as busy.

#### MacMessage
- `MacMessages` can be one of two types, either `MAC_PACKET_TYPE_DATA` or `MAC_PACKET_TYPE_ACK`.
- TODO: This deserves a detailed explanation.

#### Transmission Status Messages
Since the `Transcevier` can only be receiving from the `Channel` OR  transmitting to the `Channel`, we need to manage/schedule the transitions between these states. This is done using the following transmission status messages.
- `TransmissionRequest`
- `TransmissionConfirm`
- `TransmissionIndication`

#### SignalStart
`SignalStart` messages are sent by the `Transceiver` to the `Channel` to indicate that a transmission is taking place on the `Channel`. Among other things, the `SignalStart` message encapsulates a `MacMessage` that can be decoded by a receiving `Transceiver`.

#### SignalStop
`SignalStop` messages are sent by the `Transceiver` to the `Channel` once the transmission has concluded. Their purpose is to inform all `Transceivers` that are listening to the `Channel` that the `Channel` may now be free to transmit on. 
