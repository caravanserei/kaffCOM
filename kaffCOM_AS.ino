/* This Arduino sketch is intended for use with coffeemakers with toptronic control (e.g. my Jura Impressa S95) and allows to communicate with the coffeemaker for diagnosis, testing and modifying purposes.
  and was written in December 2014 by Oliver Krohn. Serial console should be set to include CR and NL. Hardware needed: Arduino, 4 male/female jumper wires, coffeemaker with service port.

  Additions by Arnd Schaffert (AS) in January 2016:
  - Restructured receiver section for appropriate handling of long machine response messages on 'RT:'-command.
  - A small character stand alone or preceding a regular command activates some special modes. 
  
  - Here are some regular commands (valid for Jura Impressa S90/95 only! Use at your own risk!):

  AN:01 switches coffeemaker on
  AN:02 switches coffeemaker off
  FA:04 small cup
  FA:05 two small cups
  FA:06 large cup
  FA:07 two large cups
  FA:08 steam
  FA:09 steam
  FA:0C XXL cup
  ?M1  Inkasso mode off
  ?M3  Inkasso mode on

  - Valid at least for Impressa 500/S70:
  RE:nn read word with offset 'nn' from EEPROM
  WE:nn,abcd write word 'abcd' to word-offset 'nn' into EEPROM - WARNING, may be destructive!
  RT:nn Memory dump of 16 words starting at word-offset nn (1 word = 2 byte)
  TY: shows the mask typ of main processor
  ...
  
*/

#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX TX 
#define myBaudTx 9600

#define TRUE 1
#define FALSE 0
#define RXSIZE 512 // space for longest expected answer (RT:xx has 276 byte)
#define EESIZE 0x10 // just 16x16words for now 

byte z0;
byte z1;
byte z2;
byte z3;
byte x0;
byte x1;
byte x2;
byte x3;
byte x4;
byte intra = 1;
byte inter = 7;
byte tx2rx = 10;
const unsigned long tMaxAnswr = 1000;
byte sema = TRUE;
byte semb = TRUE;
byte semc = TRUE;
byte semd = TRUE;
byte seme = TRUE;
byte rx_array[RXSIZE];
int rx_index = 0;
int buf_index = 0;
int char_index = 0;
unsigned long timerA = 0;
int eecnt = 0;
const char assignment[]="RX/TX BITS: _--1--0--_--3--2--_--5--4--_--7--6--_";
const char commandset[]="Special Modes: c.ommand-only d.ump-512w f.ull-answer h.help Back: r.egular";
//#define assignment "RX/TX BITS: _--1--0--_--3--2--_--5--4--_--7--6--_"
//#define commandset "Special Modes: c.ommand-only d.ump-512w f.ull-answer h.help Back: r.egular"
        
void setup() {
  Serial.begin(myBaudTx);
  mySerial.begin(myBaudTx);
  rx_index = 0;
  while (rx_index < 512) rx_array[rx_index++] = 0;
  rx_index = 0;
  sema = TRUE;  //print a reminder of the bit structure once per session
  semb = FALSE; //dump mode finished  
  semc = FALSE; //command mode off
  semd = FALSE; //autodump mode off
  seme = TRUE; //regular mode on 
}

void loop() {

  if (Serial.available()) {
    if (sema == TRUE)
    {
      //print bit-assignment as a reminder once per session
      sema = FALSE;
      Serial.println("WELCOME TO kaffCOM!");
      Serial.println();
      Serial.println("Regular Mode Ready.");
      Serial.println(commandset);
      Serial.println();
    }
    
    byte c = Serial.read(); // fetch user input
    
    if ((c != 'f') && (c != 'c') && (c != 'd') && (c != 'r') && (c != 'h') && (c !='?')) //something special to do?
    {
      //no: just a regular coffeemaker-command
      toCoffeemaker(c); //send each byte immediately to CM
    }
    else
    {
      //yes: special treatment required
      if ((c == 'h') || (c == '?'))
        {
        Serial.println();  
        Serial.println(commandset);
        Serial.println(assignment);
        }
      if (c == 'r') //back to regular mode
        {
        semb = FALSE;  
        semc = FALSE;
        semd = FALSE;
        seme = TRUE;  
        Serial.println();Serial.println("REGULAR MODE READY:");
        }
      if (c == 'c') //command mode = show no answers
        {
        semb = FALSE;  
        semc = TRUE;
        semd = FALSE;
        seme = FALSE;  
        Serial.println();Serial.println("SHOW ONLY COMMANDS WITHOUT RESPONSE:");
        }
      if (c == 'f') //full mode = full interpretation of messages (this was the original function of software!)
        {
        semb = FALSE;
        semc = FALSE;
        semd = FALSE;
        seme = FALSE;  
        Serial.println();Serial.println("SHOW FULL MESSAGE INTERPRETATION:");
        Serial.println(assignment);
        }
      if (c == 'd') //get a full eeprom dump automatically
        {
        semb = TRUE; //get 1st dump immediately
        semc = TRUE; //no detailed output as in command mode
        semd = TRUE; //autodump mode ON
        seme = FALSE;
        eecnt = 0;
        Serial.println();Serial.println("COLLECTING EEPROM WORDDUMP:");
        }
    }
    RewindRxCounters(); //each character you have entered could be the last of that command, be prepared for answer!
    } // serial avail. (user input)
  
  if ((semd == TRUE) && (semb == TRUE)) // auto-generate commands for EE-auto-dump only
    {
    semb = FALSE;  
    if (eecnt < EESIZE)
      {
      byte cc;  
      if (eecnt >= 0x0A)  
        cc='A'+eecnt-0x0A;
      else
        cc='0'+eecnt;  
      toCoffeemaker('R'); 
      toCoffeemaker('T'); 
      toCoffeemaker(':');  
      toCoffeemaker(cc);  
      toCoffeemaker('0');
      toCoffeemaker('\r');
      if(eecnt>0) //strange but helpful
        toCoffeemaker('\n');
      RewindRxCounters();
      //Serial.print(" : ");  
      }
    else
      {
      //Serial.println();  
      Serial.println("EEPROM DUMP COMPLETE!");  // dump ready, return to normal mode
      semd = FALSE; 
      semc = FALSE;
      semb = FALSE;
      }
    }   
    
  // from now on simply collect bytes to be received as long there is enough buffer space
  // but do this only until the longest expected answer (currently RT: memory dump with 276 message bytes) should have been safely received
  if ((rx_index < (RXSIZE-1)) && (millis() <= (timerA + tMaxAnswr)))
  {
    if (mySerial.available()) {
      //Serial.print(rx_index);Serial.print("#");//DEBUG
      rx_array[rx_index++] = mySerial.read();
      }
  }
  else
  {
    // as the whole answer message should be received by now there is plenty of time to display it
    if (rx_index >= 19) //as of now there should have arrived 20 message-bytes with e.g "ok:<CR><LF>" at least or nothing at all
    {
      if ((semc == FALSE) && (seme == FALSE)) //show in regular mode only
      {
        Serial.print("NUMBER OF BYTES RECEIVED: "); Serial.println(rx_index);
        Serial.println(assignment);
      }
      showRxBuf(rx_index);

      
      if (semd == TRUE) // in dump mode only
        {
        semb = TRUE; // one line is done eventually
        eecnt++; // prepare next block
        }
      RewindRxCounters();
      
      }
    }

}

byte showRxBuf(int i)
{
  int k = 0;
  int cs = 0;

  byte d0; byte d1; byte d2; byte d3;
  //delay (intra); byte d0 = mySerial.read();
  //delay (intra); byte d1 = mySerial.read();
  //delay (intra); byte d2 = mySerial.read();
  //delay (intra); byte d3 = mySerial.read();
  //delay (inter);

  buf_index = 0;
  char_index = 0;
  while (buf_index < i) {
    //any four coded RX-bytes give one byte of the answer
    byte d0 = rx_array[buf_index++];
    byte d1 = rx_array[buf_index++];
    byte d2 = rx_array[buf_index++];
    byte d3 = rx_array[buf_index++];

    if ((semc == FALSE) && (seme == FALSE))
    {
      // Print hex and bin values of received UART bytes
      Serial.print(d0, HEX); Serial.print(" ");
      Serial.print(d1, HEX); Serial.print(" ");
      Serial.print(d2, HEX); Serial.print(" ");
      Serial.print(d3, HEX); Serial.print("  ");

      Serial.print(d0, BIN); Serial.print(" ");
      Serial.print(d1, BIN); Serial.print(" ");
      Serial.print(d2, BIN); Serial.print(" ");
      Serial.print(d3, BIN); Serial.print("  ");
    }

    fromCoffeemaker(d0, d1, d2, d3, char_index++);
    // additional treatment for memory dump (RT:) only:
    // characters are in buffer now (dual use) instead of the received message!
    // show a memory dump derived out of all of the received payload-characters in buffer
    // characters are in buffer now instead of received message!
    
    // get answer but don't show it in command mode
    if ((semc == TRUE) && (semd == FALSE)) break;
      
    if (buf_index == i)
    { 
      // show dump answer on explicite "rt:" answer
      if (rx_array[0] == char('r') && rx_array[1] == char('t') && rx_array[2] == char(':'))
      {
        cs = 0;
        if (semd == FALSE)
            Serial.print("WORDDUMP:");//don't know address unless in dump-mode!
          else        
            {  
            Serial.print("$0");
            if(eecnt<0x10) Serial.print("0");
            Serial.print(eecnt,HEX); Serial.print("0:");      
            }
          
        // read only the payload bytes between "rt:" and "CRLF" (dont' exceed the buffer size)
        for (k = 3; k < (RXSIZE - 1); k++)
        {
          // last two bytes must be CRLF
          if (rx_array[k] == '\r' && rx_array[k + 1] == '\n')
            break;

          // insert one space any 4 Bytes thus grouping the HEX-words
          // by the way: no need to swap bytes; take them 1:1 as they are displayed
          if ((k - 3) % 4 == 0)
          {
            Serial.print(" ");
          }
          Serial.print(char(rx_array[k]));
          // build a simple checksum on displayed characters (not on the byte/word value!) just for a quick doublecheck of the display ouptput
          cs = cs + rx_array[k];
        }
        cs = cs & 0xFF ;
        Serial.print(" CS: $");
        if (cs < 0x10) Serial.print("0");
        Serial.println(cs, HEX);
       //Serial.println();
       }//RT only
    }//end of buffer
  }//still chars to be printed 
  

}

// fromCoffeemaker receives a 4 byte UART package from the coffeemaker and translates them to a single ASCII byte
byte fromCoffeemaker(byte x0, byte x1, byte x2, byte x3, int i) {
  // Print received UART bytes on console
  //    Serial.write(x0);
  //    Serial.write(x1);
  //    Serial.write(x2);
  //    Serial.write(x3);
  //    Serial.println();
  // Reads coding Bits of the 4 byte package and writes them into a new character (translates to ASCII)
  bitWrite(x4, 0, bitRead(x0, 2));
  bitWrite(x4, 1, bitRead(x0, 5));
  bitWrite(x4, 2, bitRead(x1, 2));
  bitWrite(x4, 3, bitRead(x1, 5));
  bitWrite(x4, 4, bitRead(x2, 2));
  bitWrite(x4, 5, bitRead(x2, 5));
  bitWrite(x4, 6, bitRead(x3, 2));
  bitWrite(x4, 7, bitRead(x3, 5));
  if ((semc == FALSE) && (seme == FALSE))
  {
    // Print translated ASCII character
    Serial.print(" $");
    if (x4 < 0x10) Serial.print("0");
    Serial.print(byte(x4), HEX); Serial.print("  "); 
  }
  if(semc == FALSE)
    Serial.print(char(x4));//show interpreted answer unless in command mode
  if ((semc == FALSE) && (seme == FALSE))
  {
    Serial.println();
  }
  //use already readout buffer cells to save the translated characters for secondary use (e.g. "RT:xx" buffer-dump)
  rx_array[i] = x4;
}

// toCoffeemaker translates an ASCII character to 4 UART bytes and sends them to the coffeemaker
byte toCoffeemaker(byte zeichen)
{
  z0 = 255;
  z1 = 255;
  z2 = 255;
  z3 = 255;
  // Reads bits of ASCII byte and writes it into coding bits of 4 UART bytes
  bitWrite(z0, 2, bitRead(zeichen, 0));
  bitWrite(z0, 5, bitRead(zeichen, 1));
  bitWrite(z1, 2, bitRead(zeichen, 2));
  bitWrite(z1, 5, bitRead(zeichen, 3));
  bitWrite(z2, 2, bitRead(zeichen, 4));
  bitWrite(z2, 5, bitRead(zeichen, 5));
  bitWrite(z3, 2, bitRead(zeichen, 6));
  bitWrite(z3, 5, bitRead(zeichen, 7));

  if ((semc == FALSE) && (seme == FALSE))
  {
    // Prints hex and bin values of translated UART bytes and the source ASCII character
    Serial.print(z0, HEX); Serial.print(" ");
    Serial.print(z1, HEX); Serial.print(" ");
    Serial.print(z2, HEX); Serial.print(" ");
    Serial.print(z3, HEX); Serial.print("  ");

    Serial.print(z0, BIN); Serial.print(" ");
    Serial.print(z1, BIN); Serial.print(" ");
    Serial.print(z2, BIN); Serial.print(" ");
    Serial.print(z3, BIN); Serial.print("  ");

    Serial.print(" $");
    if (zeichen < 16) Serial.print("0");
    Serial.print(byte(zeichen), HEX); Serial.print("  ");
    Serial.write(zeichen);
    Serial.println();
  }
  else
  {
    //simply repeat each command in command mode
    if (semd == FALSE) Serial.write(zeichen);
  }
  // for each character do send a complete 4 byte coded package to the coffeemaker
  delay (intra); mySerial.write(z0);
  delay (intra); mySerial.write(z1);
  delay (intra); mySerial.write(z2);
  delay (intra); mySerial.write(z3);
  delay (inter);
  
}

void RewindRxCounters(void)
{
  timerA = millis();
  rx_index = 0;
  buf_index = 0;
  char_index = 0;
}


