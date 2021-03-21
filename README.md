# nrf24_esb
Simple Enhanced ShockBurst emulation on nRF5x Platform -- (nRF52422/PCA10028 board)
It should also work with the nRF53832/PCA10040 board as well.

**NOTE:** You will need two nRF boards: one to send and one to receive.  


## Building
This project was built with Zephyr V2.5 in Ubuntu 18.04.  
You will need to modify the CMakeList.txt to select the target board and the PTX (sender) or PRX (receiver) side.  
It is assumed you have Segger's JLink installed on your build system: all Nordic dev-kit boards directly include support JLink, so its recommended to use it.

## Running and Debugging
I used the Ozone debugger from Segger, but the gdb debugger should work as well ("make debug" will kick off gdb process)
If you want to see the console messages, don't have or want Ozone, then I have included a simple "rtt.sh" script which will display the RTT console output.

Since the interaction is based on ESB, the sending side (PTX) will indicate transmit failures until it gets a reply back from the receiver (PRX).
This means that you should probably start the PRX side first, then the PTX.  The PRX side will quitely wait for the PTX side to start sending.

## Sniffing nRF24 packets
See the information at [bitcraze](https://wiki.bitcraze.io/misc:hacks:hackrf) for directions on how to sniff the nRF24 packets.  
The output of the sniffer is shown below.

```
knots:bin $ mkfifo /tmp/fifo
knots:bin$  cat /tmp/fifo | ./nrf24-btle-decoder -d 1 -l 8
nrf24-btle-decoder, decode NRF24L01+ and Bluetooth Low Energy packets using RTL-SDR v0.4

1611106561.340944 NRF24 Packet start sample 1551250,  Threshold:129,   Address: 0xE7E7E7E7E7 length:8, pid:2, no_ack:1, CRC:0x4FF1 data:01 B9 03 04 05 06 07 08 
1611106562.091037 NRF24 Packet start sample 3052466,  Threshold:226,   Address: 0xE7E7E7E7E7 length:8, pid:1, no_ack:1, CRC:0x6CDE data:01 BC 03 04 05 06 07 08 
1611106562.338714 NRF24 Packet start sample 3552982,  Threshold:-875,  Address: 0xE7E7E7E7E7 length:8, pid:2, no_ack:1, CRC:0x8E37 data:01 BD 03 04 05 06 07 08 
1611106562.338991 NRF24 Packet start sample 3554328,  Threshold:-730,  Address: 0xE7E7E7E7E7 length:8, pid:2, no_ack:1, CRC:0x8E37 data:01 BD 03 04 05 06 07 08 
1611106563.092743 NRF24 Packet start sample 5054117,  Threshold:-566,  Address: 0xE7E7E7E7E7 length:8, pid:1, no_ack:1, CRC:0xBAD4 data:01 C0 03 04 05 06 07 08 
1611106563.340929 NRF24 Packet start sample 5554511,  Threshold:37,    Address: 0xE7E7E7E7E7 length:8, pid:2, no_ack:1, CRC:0x583D data:01 C1 03 04 05 06 07 08 
1611106564.587363 NRF24 Packet start sample 8056576,  Threshold:-340,  Address: 0xE7E7E7E7E7 length:8, pid:3, no_ack:1, CRC:0x871E data:01 C6 03 04 05 06 07 08 
1611106564.843243 NRF24 Packet start sample 8557092,  Threshold:-1261, Address: 0xE7E7E7E7E7 length:8, pid:0, no_ack:1, CRC:0x65F7 data:01 C7 03 04 05 06 07 08 
1611106565.340703 NRF24 Packet start sample 9557865,  Threshold:-346,  Address: 0xE7E7E7E7E7 length:8, pid:2, no_ack:1, CRC:0xCB90 data:01 C9 03 04 05 06 07 08 
1611106566.341892 NRF24 Packet start sample 11559567, Threshold:-446,  Address: 0xE7E7E7E7E7 length:8, pid:2, no_ack:1, CRC:0x0A56 data:01 CD 03 04 05 06 07 08 
```

 ![Hardware setup](https://github.com/foldedtoad/nrf24_esb/blob/master/docs/gnuradio-companion.png)
