#include <math.h>               //Library  for funciton ROUND()
#include <SoftwareSerial.h>     //Library to make possible assign other pins as RX/TX
#include <Timer.h>              //Library to obtain data from the OBD II via 'pulse' method and not from normal delay normal (Arduino).

#define RxD 2                //Arduino pin connected to Tx of HC-05
#define TxD 3                //Arduino pin connected to Rx of HC-05
SoftwareSerial blueToothSerial(RxD,TxD); //SoftwareSerial mySerial(RX,TX); //3.3v between Arduino´s TX and module´s RX

char inByte;
boolean obd_error_flag;      // Variable for OBD connection error
#define OBD_CMD_RETRIES 3    // Number of retries for each OBD command in case of not receive prompt '>' char
/*
 * SETUP
 */
void setup()
{
 Serial.begin(9600);
 blueToothSerial.begin(38400);    //HC-05 AT mode baud rate is 38400

 pinMode(RxD, INPUT);
 pinMode(TxD, OUTPUT);
 
 obd_error_flag=false;                // OBD error flag is false
 send_OBD_cmd("ATZ");                 // send to OBD ATZ, reset
 delay(1000);

Serial.print("sd1,0;");delay(10);
Serial.print("ss2/7;");delay(10);
send_OBD_cmd("ATE0");             // ECHO OFF (This way OBDII does not echo back the sent commands)
delay(1000);

 Serial.print("sd1,0;");delay(10);
 Serial.print("ss2/2;");delay(10);
 send_OBD_cmd("ATSP0");              //send ATSP0, protocol auto
 delay(2000);

//Para quado se quiser obter os PID´s disponíveis;
Serial.print("sd1,0;");delay(10);
Serial.print("ss4/7;");delay(10);
send_OBD_cmd("0100");     //send 0100, retrieve available pid's 00-19
delay(2000);
//
Serial.print("sd1,0;");delay(10);
Serial.print("ss5/7;");delay(10);
send_OBD_cmd("0120");     //send 0120, retrieve available pid's 20-39
delay(2000);
//
Serial.print("sd1,0;");delay(10);
Serial.print("ss6/7;");delay(10);
send_OBD_cmd("0140");     //send 0140, retrieve available pid's 40-??
delay(2000);

//Teste para obter valor de RPM:
Serial.print("sd1,0;");delay(10);
Serial.print("ss7/7;");delay(10);
send_OBD_cmd("010C1");    //send 010C1, RPM cmd
delay(1000);

Serial.print("sc;"); delay(10);
Serial.print("sd0,0;"); delay(10);
Serial.print("ssRPM:;"); delay(10);

} 
/*
 * Send ODB command for initialization
 */
void send_OBD_cmd(char *obd_cmd){
  char recvChar;
  boolean prompt;
  int retries;
 
   if (!(obd_error_flag)){                                        // if no OBD connection error    
    prompt=false;
    retries=0;
    while((!prompt) && (retries<OBD_CMD_RETRIES)){                // while no prompt and not reached OBD cmd retries
      blueToothSerial.print(obd_cmd);                             // send OBD cmd
      blueToothSerial.print("\r");                                // send cariage return

      while (blueToothSerial.available() <= 0);                   // wait while no data from ELM
      
      while ((blueToothSerial.available()>0) && (!prompt)){       // while there is data and not prompt
        recvChar = blueToothSerial.read();                        // read from elm
        Serial.println(recvChar);
       if (recvChar==62) prompt=true;                             //if received char is '>' then prompt is true
     }
      retries=retries+1;                                          //increase retries
      delay(2000);
    }
    if (retries>=OBD_CMD_RETRIES) {                               // if OBD cmd retries reached
      obd_error_flag=true;                                        // obd error flag is true
       digitalWrite(13,HIGH);
    }
  }
}

void loop() {
Serial.println("Open Loop");

Serial.println("End Loop");
}
