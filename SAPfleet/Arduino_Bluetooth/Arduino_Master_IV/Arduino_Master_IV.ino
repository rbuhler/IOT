#include <SoftwareSerial.h>
#define RxD 2                //Arduino pin connected to Tx of HC-05
#define TxD 3                //Arduino pin connected to Rx of HC-05

#define RPM  1
#define RPM_CODE "410C"
#define TEMP 2
#define TEMP_CODE "4105"
#define SPD  3
#define SPD_CODE "410D"
#define LOAD 4
#define LOAD_CODE "4104"

SoftwareSerial blueToothSerial(RxD,TxD); 
int i;
char inByte;
boolean linked = true;
char bufferIn[15];
int indx;

void setup()
{
Serial.begin(9400); 
blueToothSerial.begin(38400);
pinMode(RxD, INPUT);
pinMode(TxD, OUTPUT);

send_OBD_cmd("ATZ");              // send to OBD ATZ, reset
delay(1000);
send_OBD_cmd("ATE0");             // ECHO OFF (Para o OBDII não devolver os mesmos comandos enviados)
delay(1000);

} 

void loop() {

  // OBD linked?
  if (linked){
        
    if (blueToothSerial.available()) {        //PORTA DO BLUETOOTH RX1 + TX1 (7 e 8)  
      inByte = blueToothSerial.read();
      buffer_read( inByte );   
    }else{
      
      if(i>0){Serial.print(",");};
      
      i =i+1;
      send();
    }
  }
}

  void send(){
    int wait = 500;
//    List from 1-20
//    blueToothSerial.print("01001");
//    blueToothSerial.print("\r");

    switch(i){  
      case RPM:
        // RPM
        blueToothSerial.print("010C1");
        blueToothSerial.print("\r");      
        break;
      case TEMP:
        // Temperature
        blueToothSerial.print("01051");
        blueToothSerial.print("\r");
        break;
      case SPD:
        // Speed
        blueToothSerial.print("010D1");
        blueToothSerial.print("\r");
        break;
      case LOAD:
        // Engine Load
        blueToothSerial.print("01041");
        blueToothSerial.print("\r");
        break;
      default:
        i = 0;
    } 
    delay(wait);

  }
  void buffer_read(char char_read){

    // Exceptions
    if(char_read == 32){                // Character SPACE
      
    } else {
      if( char_read == 62){             // Character '>'
      
      } else {                          // No Exception
        if( char_read == 13 || char_read ==10){
          
        }else{
          indx = indx +1;
          switch(i){
            case RPM:
//              RPM_CODE;
              Serial.write(char_read);
              break;
            case TEMP:
              Serial.write(char_read);
              char bufferIn[indx];
              break;
            case SPD:
//              SPD_CODE;
              Serial.write(char_read);
              break;
            case LOAD:
//              LOAD_CODE;
              Serial.write(char_read);
              break;
            default:
              indx = 0;
              memset(bufferIn,0,sizeof(bufferIn));
          }
        }
      }
    }
  }

  void coerce_data(){
    
  }

/*
 * Send ODB command for initialization
 */
  void send_OBD_cmd(char *obd_cmd){
    #define RETRY 10
    
    char recvChar;
    boolean prompt;
    int retries;
   
    retries= 0;
    prompt = true;
    while((!prompt) && (retries<RETRY)){                          // while no prompt and not reached OBD cmd retries
      blueToothSerial.print(obd_cmd);                             // send OBD cmd
      blueToothSerial.print("\r");                                // send cariage return
      while (blueToothSerial.available() <= 0);                   // wait while no data from ELM
        
      while ((blueToothSerial.available()>0) && (!prompt)){       // while there is data and not prompt
        recvChar = blueToothSerial.read();                        // read from elm
       if (recvChar==62) prompt=true;                             // if received char is '>' then prompt is true
      }
      delay(2000);
    }
    if (retries>=RETRY) {                                         // if OBD cmd retries reached
      Serial.println("@ Error connecting OBD");
    }
  }
