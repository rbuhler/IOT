#include <SoftwareSerial.h>
#define RxD 2                //Arduino pin connected to Tx of HC-05
#define TxD 3                //Arduino pin connected to Rx of HC-05

#define RPM  1
#define RPM_CODE "010C"
#define TEMP 2
#define TEMP_CODE "0105"
#define SPD  3
#define SPD_CODE "010D"
#define LOAD 4
#define LOAD_CODE "0104"

SoftwareSerial blueToothSerial(RxD,TxD); 
//int i;
char inByte;
boolean linked = true;
char bufferIn[15];
int indx;
boolean prompt = false;

String csv[3];
boolean record;

void setup()
{
Serial.begin(38400); 
blueToothSerial.begin(38400);
pinMode(RxD, INPUT);
pinMode(TxD, OUTPUT);

record = false;
send_OBD_cmd("ATZ");              // send to OBD ATZ, reset
send_OBD_cmd("ATE0");             // ECHO OFF (Para o OBDII não devolver os mesmos comandos enviados)

} 

void loop() {
  get_obd();

  Serial.println(csv[0]);
}

/*
 * OBD II
 */
  void get_obd(){

    record = true;
    send_OBD_cmd(RPM_CODE);
    Serial.print("CSV : ");
    Serial.println(csv[0]);
    send_OBD_cmd(TEMP_CODE);
    Serial.print("CSV : ");
    Serial.println(csv[0]);
    send_OBD_cmd(SPD_CODE);
    Serial.print("CSV : ");
    Serial.println(csv[0]);
    send_OBD_cmd(LOAD_CODE);
    Serial.print("CSV : ");
    Serial.println(csv[0]);
  }    
  
/*
 * Send ODB command for initialization
 */
  void send_OBD_cmd(char *obd_cmd){
    
    char recvChar;
    int retries;
    int RETRY;

    csv[0] = "";
    
   Serial.print("Command : ");
   Serial.println(obd_cmd);
   
    retries= 0;
    RETRY = 10;
    prompt = false;
    while((!prompt) && (retries < RETRY)){                        // while no prompt and not reached OBD cmd retries
      blueToothSerial.print(obd_cmd);                             // send OBD cmd
      blueToothSerial.print("\r");                                // send cariage return

      Serial.print("Answer : ");
      delay(500);
      while (blueToothSerial.available() <= 0);                   // wait while no data from ELM
        
      while ((blueToothSerial.available()>0) && (!prompt)){       // while there is data and not prompt
        recvChar = blueToothSerial.read();                        // read from elm 
        if (recvChar==62){
          prompt=true;                                            // if received char is '>' then prompt is true
        }else{
          Serial.print(recvChar);
          if (recvChar == 32 || recvChar == 10 | recvChar == 13){
            
          }else{
            if(record){ csv[0] += recvChar; };
          }
        }
       
      }
      retries += 1;
      delay(1000);
      Serial.println("");
    }
    if (retries>=RETRY) {                                         // if OBD cmd retries reached
      Serial.println("@ Error connecting OBD");
    }
  }  
