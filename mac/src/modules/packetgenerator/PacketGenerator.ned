package mac.modules.packetgenerator;

simple PacketGenerator
{
    parameters:
        // The packet inter-generation time distribution.
        volatile double iatDistribution @unit(s);
        
        // The distribution of the size of the generated message in bytes.
        volatile double msgSizeDistribution @unit(B);
        
        // The id of this transmitter node - must be the same value given to the other submodules of the compound module.
        int senderId;
        
        // The id of the receiver node, there is only one receiver in the simulation.
        int receiverId;
    
    gates:
        // The gates to and from the MAC module.
        input fromMac;
        output toMac;
}
