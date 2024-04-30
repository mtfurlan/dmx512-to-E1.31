# DMX to E1.31 ACN relay

things like [Artemis](https://www.artemisspaceshipbridge.com/) bake their own ftd2xx.dll in, and can only use ftdi stuff for dmx

Things like wled accept dmx over the network

This repacks the dmx packets into E1.31 and does udp multicast with them

This is really just hacking together some other people's stuff
* I documented and update the E1.31 struct from [forkineye/ESPAsyncE131](https://github.com/forkineye/ESPAsyncE131/blob/master/ESPAsyncE131.h)
* I mostly just reused the initilization stuff from [tigoe/sACNSource](https://github.com/tigoe/sACNSource/blob/main/src/sACNSource.cpp)
* the [actual ANSI E1.31 spec](https://tsp.esta.org/tsp/documents/docs/ANSI_E1-31-2018.pdf) has some useful things too
