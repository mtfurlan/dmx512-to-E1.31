# DMX to E1.31 ACN relay

things like [Artemis](https://www.artemisspaceshipbridge.com/) bake their own ftd2xx.dll in, and can only use ftdi stuff for dmx

Things like wled accept dmx over the network

This repacks the dmx packets into E1.31 and does udp multicast with them
