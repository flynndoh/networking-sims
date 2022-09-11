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

sim-time-limit = 1000s
simtime-resolution = ps
**.*.iatDistribution = 10ms
**.*.msgSizeDistribution = 64B

**.*.txPowerDBm = 0dBm
**.*.bitRate = 250000bps
**.*.csThreshDBm = -50dBm
**.*.noisePowerDBm = -120dBm
**.*.turnAroundTime = 1ms
**.*.pathLossExponent = 4
**.*.csTime = 125us
**.*.backoffDistribution = exponential(3ms)
**.*.retransmissionDistribution = exponential(10ms)
**.*.bufferSize = 5
**.*.maxBackoffs = 5
```

### CSMA with 3 Transceivers Scenario
The scenario's variables can be viewed and tweaked in the `simulations/csma_3_nodes/omnetpp.ini` file. If you would like to alter the topography of the network, you can do so by adding new nodes and connections in the `simulations/csma_3_nodes/csma_3_nodes.ned` file.

##### Variables
```
**.tx_packetgenerator1.receiverId = 0
**.tx_packetgenerator1.senderId = 1
**.tx_packetgenerator2.receiverId = 0
**.tx_packetgenerator2.senderId = 2
**.tx_packetgenerator3.receiverId = 0
**.tx_packetgenerator3.senderId = 3

**.*.maxAttempts = 5
**.*.acknowledgementSize = 100b

**.tx_txrx1.nodeId = 1
**.tx_txrx1.nodeXPosition = 10
**.tx_txrx1.nodeYPosition = 10

**.tx_txrx2.nodeId = 2
**.tx_txrx2.nodeXPosition = 30
**.tx_txrx2.nodeYPosition = 10

**.tx_txrx3.nodeId = 3
**.tx_txrx3.nodeXPosition = 50
**.tx_txrx3.nodeYPosition = 10

**.rx_txrx.nodeId = 0
**.rx_txrx.nodeXPosition = 0
**.rx_txrx.nodeYPosition = 0
```

### Modules

#### Packet Generator


#### Packet Sink


#### Medium Access Control (MAC)


#### Transceiver (Receiver + Transmitter)


#### Channel


### Messages/Packets

#### AppMessage


#### AppResponse


#### CSRequest (Carrier Sense Request)


#### CSResponse (Carrier Sense Response)


#### MacMessage


#### SignalStart


#### SignalStop


#### TransmissionConfirm


#### TransmissionIndication


#### TransmissionRequest
