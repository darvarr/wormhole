# wormhole
A mono-directional wormhole implementation.

The sniffer folder contains the Contiki code for the sniffer part. It turns a mote into a Sniffer, which sends all the incoming frames over the serial port where the mote is connected. 

The replayer folder contains the Contiki code for the replayer part. It turns the mote into a Replayer, which replay all the frames coming from the serial port where the mote is connected. When flashed, the replayer is always in promiscuous mode. The replayer is also able to ACK each frame coming from a specific set of victims. The MAC address of the victims can be specified in the nullReplayerRDC.c file. A special macro to enable the ACK functionality, NULLREPLAYERRDC_CONF_FORGE_802154_ACK, is present in the project-conf, and is enabled by default.

Both of them are intended for the TI Launchpad CC2650 platform. You should add the path for the contiki folder in both sniffer and replayer Makefiles before flashing them.

The client script is intended to be used in the sniffer side. It should be used by adding server IP and Port, and the sniffer serial port name.

The server script is to be user in the replayer side. It shuold be used by adding the server Port and the replayer serial port name.

The scripts ctrl0 and ctrl1 can be used for turning ON/OFF the replayer Radio, which is ON by default. As an example, they can be invoked in a bash script, to schedule the replayer.
