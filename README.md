# kaffCOM
Infrared and Serial Interface for certain Swiss-made Coffeemakers.

This hardware and software is intended to read and control some coffeemakers of that well known Swiss brand (and some others) which make use of an infrared coupled diagnostic port. The same circuit can read out direct coupled (plugged-in) machines as well as long as they use the same TTL-based/5V UART protocol and physical pin-assigment. If the four-pin header doesn't fit your machine, maybe you find the appropriate pin-assignment of other machines and can make a simple adaptor of the relevant signals RXD, TXD and GND.

![kaffCom-at-work](kaffCOM-im-Einsatz.JPG?raw=true "kaffCOM at Work!")

kaffCOM software is based on the original Arduino-scripts coffeemaker-communication-tool.ino and EEPROM_tool.ino written by OLIVER KROHN in December 2014 which you may find on GITHUB, too: https://github.com/oliverk71/Coffeemaker-Payment-System. 

The adopted kaffCOM.ino is intended to be used with an Arduino NANO but should work with other Arduinos, too.

You may want to build all of the electronics inlcuding the Arduino into my kaffCOM housing which is made of two self locking parts you can print with your 3D-Printer. When you use the infrared interface just place the box with the bigger rectangular window without a gap directly in front of the machine's infrared window. It should fit exactly into the hollow behind the keyboard hatch. When you use the pin-header for machines with a wired diagnostic interface you either may want to use spring contacts with crown shaped tips, or you simply make a short adaptor cable with the appropriate plug on machine side and lead the three needed wires through the smaller holes. The contact bars in those machines look very similar to some JST XH four-pin types.

07.03.2016 - Created this repository to share the contents.

DISCLAIMER: This project was made for fun and for private purposes only and it comes as it is without any warranty or garanteed functionality. Use at your own risk including the risk of damaging your devices.

Arnd Schaffert, Germany
