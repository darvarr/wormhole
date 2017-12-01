# wormhole
A wormhole implementation for Contiki.

The sniplayer code is combination of the capability of a sniffer and a replayer. 

The sniplayer folder contains the Contiki code for the sniplayer, which is one of the end-points of the wormhole. It turns the mote into a Sniplayer. The sniffer functionality makes it capable of send, using the serial port, each frame received from the mote. The replayer functionality makes it capable of replay all the frames coming from the serial port where the mote is connected. When flashed, the sniplayer is always in promiscuous mode. The sniplayer is also able to ACK each frame coming from a specific set of victims. The MAC address of the victims can be specified in the null_sniplayRDC.c file. A special macro to enable the ACK functionality, NULLSNIPLAYRDC_CONF_FORGE_802154_ACK, is present in the project-conf, and is enabled by default.

The sniplayer code is intended for the TI Launchpad CC2650 platform. You should add the path for the contiki folder in the Makefile before flashing the mote.

The wormhole end-points can be connected using a cable connection or a remote one.

The scripts ctrl0 and ctrl1 can be used for turning ON/OFF the sniplayer Radio, which is ON by default. As an example, they can be invoked in a bash script, to schedule the wormhole.
