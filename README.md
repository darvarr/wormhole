# wormhole
A mono-directional wormhole implementation

The client script is intended to be used in the sniffer side. It should be used by adding server IP and Port, and the sniffer serial port name.
The server script is to be user in the replayer side. It shuold be used by adding the server Port and the replayer serial port name.
The scripts ctrl0 and ctrl1 can be used for turning ON/OFF the replayer Radio, which is ON by default. As an example, they can be invoked via SSH in a bash script, to schedule the replayer.
