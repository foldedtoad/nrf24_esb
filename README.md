# nrf24_esb
Simple Enhanced ShockBurst emulation on nRF5x Platform -- (nRF52422/PCA10028 board)
It should also work with the nRF53832/PCA10040 board as well.

NOTE: You will need two nRF boards: one to send and one to receive.  
Since the protocol is ESB with ACKs the sending side (PTX) will indicate failure until it gets ACKs back from the receiver (PRX).

