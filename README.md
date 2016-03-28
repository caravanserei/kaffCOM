# kaffCOM
Infrared and Serial Interface for certain Swiss-made Coffeemakers.

This hardware and software is intended to read and control some coffeemakers of that well known Swiss brand (and some others) which make use of an infrared coupled diagnostic port. The same circuit can read out direct attached (plug-in) machines as well as long as they use the same TTL-based/5V UART protocol and logical interface. If the four-pin header doesn't fit your machine, maybe you find the appropriate pin-assignment of other machines and can make a simple adaptor for the relevant signals RXD, TXD and GND.

![kaffCom-at-work](./pictures/kaffCOM-in use.JPG?raw=true "kaffCOM at Work!")
Picture by Arnd Schaffert

kaffCOM software is based on the original Arduino-scripts "coffeemaker-communication-tool.ino" and "EEPROM_tool.ino" written by OLIVER KROHN in December 2014 which you can find on GITHUB, too: https://github.com/oliverk71/Coffeemaker-Payment-System. 

The adopted kaffCOM.ino (developed with IDE 1.6.7) is intended to be used with an Arduino NANO v3 but should work with other Arduinos, too. Be aware of low-cost NANOs from Far East with faked FTDI-chips! These are not all fakes, of course. In fact I have five well manufactured but still cheap boards with genuine chips and two least-cost boards with fakes on my bench right now. Each from different China-based sellers. Beside ORIGINAL Arduino/Genuino boards, those boards which come with a CH340G USB-device do work well, too.

You may want to build all of the electronics inlcuding the Arduino into my kaffCOM housing which is made of two self locking parts you can print with your 3D-Printer (Pretty nice previewer on GIT by now :-). When you use the infrared interface just place the box with the bigger rectangular window flat in front of the machine's infrared window: It should fit exactly into the hollow behind the keyboard hatch. When you use the pin-header for machines with a wired diagnostic interface you either may want to use spring contacts (as visible in the picture above) with crown shaped tips, or you simply make an adaptor cable with the appropriate plug on machine side and lead the three or four neccessary wires through the smaller holes. With a knot or a tie wrap on the inside as strain relief they make a fine wire guide, too and the optical section will not be hindered. The contact bars in those machines look very similar to JST XH four-pin types, but I don't know for sure.

If you are interested in the technical details please have a closer look at the not too complicated wiring diagram of kaffCOM and the pictures and oscillograms of the project.

By the way, the prototype you see here is made with the "toner-direct-method" for both the copper and the placement side.

07.03.2016 - Created this repository to share the contents.

DISCLAIMER: This project was made for fun and for private purposes only and it comes as it is without any warranty or guaranteed properties. Use at your own risk including the risk of damaging all of your devices and loss of data.

Arnd Schaffert, March 2016

Munich, Germany




