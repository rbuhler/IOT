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
 *  RPM variables
 */
boolean rpm_error_flag;      // Variable for RPM error
boolean rpm_retries;         // Variable for RPM cmd retries
unsigned int rpm_val;        // Variables for RPM
byte rpmdata   = 0;          // Marker that new data are available
Timer tRPM;
#define RPM_CMD_RETRIES 5    // Number of retries for RPM command
/* 
 *  Temperature variables
 */
boolean temp_error_flag;      // Variable for Temperatura error
boolean temp_retries;         // Variable for Temperatura cmd retries
int temp_val;                 // Variables for Temperatura
byte tempdata  = 0;           // Marker that new data are available
Timer tTemp;                  
#define TEMP_CMD_RETRIES 5    // Number of retries for Temperatura command
/* 
 *  Speed variables
 */
boolean speed_error_flag;      // Variable for Speed error
boolean speed_retries;         // Variable for Speed cmd retries
unsigned int speed_val;        // Variables for Speed Speed
byte speeddata = 0;            // Marker that new data are available                     
Timer tSpeed;
#define SPEED_CMD_RETRIES 5    // Number of retries for Speed command
/* 
 *  Load variables
 */
boolean load_error_flag;      // Variable for LoadLo error
boolean load_retries;         // Variable for Load cmd retries
int load;                     // Variables 
byte loaddata  = 0;           // marker that new data are available
Timer tLoad;
#define LOAD_CMD_RETRIES 5    // Number of retries for Load command
/*
 * SETUP
 */
void setup()
{
 Serial.begin(9600);
 blueToothSerial.begin(38400);    //HC-05 AT mode baud rate is 38400

 pinMode(RxD, INPUT);
 pinMode(TxD, OUTPUT);
 /*
  * Read OBD data
  */
 tRPM.every(250,get_rpm);                  //Every 250ms reads RPM value from OBD
 tTemp.every(250,get_temp);                //Every 250ms reads TEMP value from OBD
 tSpeed.every(250,get_speed);              //Every 250ms reads Speed value from OBD
 tLoad.every(250,get_load);                //Every 250ms reads LOAD value from OBD

 obd_error_flag=false;                // OBD error flag is false
 send_OBD_cmd("ATZ");                 // send to OBD ATZ, reset
 delay(1000);

//Serial.print("sd1,0;");delay(10);
//Serial.print("ss2/7;");delay(10);
//send_OBD_cmd("ATE0");             // ECHO OFF (Para o OBDII não devolver os mesmos comandos enviados)
//delay(1000);

 Serial.print("sd1,0;");delay(10);
 Serial.print("ss2/2;");delay(10);
 send_OBD_cmd("ATSP0");              //send ATSP0, protocol auto
 delay(2000);

//Para quado se quiser obter os PID´s disponíveis;
//Serial.print("sd1,0;");delay(10);
//Serial.print("ss4/7;");delay(10);
//send_OBD_cmd("0100");     //send 0100, retrieve available pid's 00-19
//delay(2000);
//
//Serial.print("sd1,0;");delay(10);
//Serial.print("ss5/7;");delay(10);
//send_OBD_cmd("0120");     //send 0120, retrieve available pid's 20-39
//delay(2000);
//
//Serial.print("sd1,0;");delay(10);
//Serial.print("ss6/7;");delay(10);
//send_OBD_cmd("0140");     //send 0140, retrieve available pid's 40-??
//delay(2000);

//Teste para obter valor de RPM:
//Serial.print("sd1,0;");delay(10);
//Serial.print("ss7/7;");delay(10);
//send_OBD_cmd("010C1");    //send 010C1, RPM cmd
//delay(1000);

//Serial.print("sc;"); delay(10);
//Serial.print("sd0,0;"); delay(10);
//Serial.print("ssRPM:;"); delay(10);

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
       if (recvChar==62) prompt=true;                             //if received char is '>' then prompt is true
     }
      retries=retries+1;                                          //increase retries
      delay(2000);
    }
    if (retries>=OBD_CMD_RETRIES) {                               // if OBD cmd retries reached
      obd_error_flag=true;                                        // obd error flag is true
      Serial.println("@ Error connecting OBD");
    }
  }
}

void loop() {

tRPM.update();              //update of timer for calling get_rpm
//Serial.println("@ RPM OUT : " + rpm_val);
if (rpmdata!=rpm_val){
    rpmdata=rpm_val;
    Serial.println("@ RPM IN : " + rpm_val);
    } 
    
//tTemp.update();            //update of timer for calling get_temp
//if (tempdata!=temp_val){
//    tempdata=temp_val;
//    Serial.println("Temperature : " + temp_val);
//    } 
//
//tSpeed.update();          // update of timer for calling get_speed
//if (speeddata!=speed_val){
//    speeddata=speed_val;
//    Serial.println("Speed : " + speed_val);
//    } 
//
//tLoad.update();           // update of timer for calling load_calc
//if (loaddata!=load){
//    loaddata=load;
//    Serial.println("Load : " + load);
//    } 
}
// ---------------------------------------------------------------------------------------------------
/*
 * RPM
 */
void get_rpm(){
   boolean prompt,valid;  
   char recvChar;
   char bufin[15];
   int i;

   if (!(obd_error_flag)){                                   //if no OBD connection error
     valid  = false;
     prompt = false;
     blueToothSerial.print("010C1");                        //send to OBD PID command 010C is for RPM, the last 1 is for ELM to wait just for 1 respond (see ELM datasheet)
     blueToothSerial.print("\r");                           //send to OBD cariage return char

     while (blueToothSerial.available() <= 0);              //wait while no data from ELM
     
     i = 0;
     while ((blueToothSerial.available()>0) && (!prompt)){  //if there is data from ELM and prompt is false
       recvChar = blueToothSerial.read();                   //read from ELM
       if ((i<15)&&(!(recvChar==32))) {                     //the normal respond to previus command is 010C1/r41 0C ?? ??>, so count 15 chars and ignore char 32 which is space
         bufin[i]=recvChar;                                 //put received char in bufin array
         i=i+1;                                             //increase i
       }  
       if (recvChar==62) prompt=true;                       //if received char is 62 which is '>' then prompt is true, which means that ELM response is finished 
     }
   
    if ((bufin[6]=='4') && (bufin[7]=='1') && (bufin[8]=='0') && (bufin[9]=='C')){ //if first four chars after our command is 410C
      valid=true;                                                                  //then we have a correct RPM response
      Serial.println("@ RPM response Ok");
    } else {
      valid=false;                                                                 //else we dont
      Serial.println("@ RPM response NOT Ok");
    }
    if (valid){                                                                    //in case of correct RPM response
      rpm_retries=0;                                                               //reset to 0 retries
      rpm_error_flag=false;    
/* Start calculation of real RPM value
   RPM is coming from OBD in two 8bit(bytes) hex numbers for example A=0B and B=6C
   the equation is ((A * 256) + B) / 4, 
   so 0B=11 and 6C=108 so rpm=((11 * 256) + 108) / 4 = 731 a normal idle car engine rpm */
      rpm_val=0;                                                                                      
      for (i=10;i<14;i++){                        //in that 4 chars of bufin array which is the RPM value
        if ((bufin[i]>='A') && (bufin[i]<='F')){  //if char is between 'A' and 'F'
          bufin[i]-=55;                           //'A' is int 65 minus 55 gives 10 which is int value for hex A
        }  
        if ((bufin[i]>='0') && (bufin[i]<='9')){  //if char is between '0' and '9'
          bufin[i]-=48;                           //'0' is int 48 minus 48 gives 0 same as hex
        }
        rpm_val=(rpm_val << 4) | (bufin[i] & 0xf);        //shift left rpm 4 bits and add the 4 bits of new char
      }
      rpm_val=rpm_val >> 2;                               //finaly shift right rpm 2 bits, rpm=rpm/4
    }
    Serial.println("@ Connected and RPM value is :" + rpm_val);
  }
  if (!valid){                                  //in case of incorrect RPM response
    rpm_error_flag=true;                        //set rpm error flag to true
    rpm_retries+=1;                             //add 1 retry
    rpm_val=0;     
    //set rpm to 0
    //Serial.println("RPM_ERROR");
    if (rpm_retries>=RPM_CMD_RETRIES) obd_error_flag=true;  //if retries reached RPM_CMD_RETRIES limit then set obd error flag to true
  }
  
  if ((obd_error_flag==true)){               //if no OBD connection error
    rpm_val=0; 
    rpm_retries=0;                 
  }
} // RPM
// ---------------------------------------------------------------------------------------------------
/*
 * Temperature
 */
void get_temp(){
   boolean prompt,valid;  
   char recvChar;
   char bufin[12];
   int i;

  if (!(obd_error_flag)){                                   //if no OBD connection error
     valid=false;
     prompt=false;
     blueToothSerial.print("0105");                         //send to OBD PID command 0105 is for Temperature
     blueToothSerial.print("\r");                           //send to OBD cariage return char
     while (blueToothSerial.available() <= 0);              //wait while no data from ELM
     i=0;
     while ((blueToothSerial.available()>0) && (!prompt)){  //if there is data from ELM and prompt is false
       recvChar = blueToothSerial.read();                   //read from ELM
       if ((i<12)&&(!(recvChar==32))) {                     //the normal respond to previus command is 0105/r41 05 ??>, so count 12 chars and ignore char 32 which is space
         bufin[i]=recvChar;                                 //put received char in bufin array
         i=i+1;                                             //increase i
       }  
       if (recvChar==62) prompt=true;                       //if received char is 62 which is '>' then prompt is true, which means that ELM response is finished 
     }

    if ((bufin[5]=='4') && (bufin[6]=='1') && (bufin[7]=='0') && (bufin[8]=='5')){ //if first four chars after our command is 4105
      valid=true;                                          //then we have a correct TEMP response
    } else {
      valid=false;                                         //else we dont
    }
    if (valid){                                            //in case of correct TEMP response
      temp_retries=0;                                      //reset to 0 retries
      temp_error_flag=false;                               //set rpm error flag to false
    /* 
     * Calculation 
     */
    String tempHex(bufin[9]); 
    String tempHex2(bufin[10]); 
    String tempHexTotal=tempHex+tempHex2;
    temp_val=hexToDec(tempHexTotal);
    temp_val-=40; 
     }
    if (!valid){                                           //in case of incorrect TEMP response
      temp_error_flag=true;                                //set rpm error flag to true
      temp_retries+=1;                                     //add 1 retry
      temp_val=0;                                          //set temperatura to 0

      //Serial.println("RPM_ERROR");
      if (temp_retries>=TEMP_CMD_RETRIES) obd_error_flag=true;  //if retries reached TEMP_CMD_RETRIES limit then set obd error flag to true
    }    
      if ((obd_error_flag==true)){temp_val=0; temp_retries=0;}     //if no OBD connection error !GrcByte!   
  }
} // Temperature

/*
 * Speed
 */
void get_speed(){
   boolean prompt,valid;  
   char recvChar;
   char bufin[12];
   int i;

  if (!(obd_error_flag)){                                   //if no OBD connection error
     valid=false;
     prompt=false;
     blueToothSerial.print("010D");                        //send to OBD PID command 010D is for speed
     blueToothSerial.print("\r");                           //send to OBD cariage return char
     while (blueToothSerial.available() <= 0);              //wait while no data from ELM
     i=0;
     while ((blueToothSerial.available()>0) && (!prompt)){  //if there is data from ELM and prompt is false
       recvChar = blueToothSerial.read();                   //read from ELM
       if ((i<12)&&(!(recvChar==32))) {                     //the normal respond to previus command is 010D/r41 0D ??>, so count 12 chars and ignore char 32 which is space
         bufin[i]=recvChar;                                 //put received char in bufin array
         i=i+1;                                             //increase i
       }  
       if (recvChar==62) prompt=true;                       //if received char is 62 which is '>' then prompt is true, which means that ELM response is finished 
     }

    if ((bufin[5]=='4') && (bufin[6]=='1') && (bufin[7]=='0') && (bufin[8]=='D')){ //if first four chars after our command is 410D
      valid=true;                                                                  //then we have a correct speed response
    } else {
      valid=false;                                                                 //else we dont
    }
    if (valid){                                                                    //in case of correct speed response
      speed_retries=0;                                                               //reset to 0 retries
      speed_error_flag=false;                                                        //set rpm error flag to false
      
    /* 
     * Calculation 
     */
    String speedHex(bufin[9]); 
    String speedHex2(bufin[10]); 
    String speedHexTotal=speedHex+speedHex2;
    speed_val=hexToDec(speedHexTotal);
     }
    if (!valid){                                             // in case of incorrect speed response
      speed_error_flag=true;                            // set rpm error flag to true
      speed_retries+=1;                                 // add 1 retry
      speed_val=0;                                               // set speed to 0
      //Serial.println("RPM_ERROR");
      if (speed_retries>=SPEED_CMD_RETRIES) obd_error_flag=true;  //if retries reached SPEED_CMD_RETRIES limit then set obd error flag to true
    }    
      if ((obd_error_flag==true)){speed_val=0; speed_retries=0;} //if no OBD connection error !GrcByte!
   
  }
} // Speed

/*
 * Engine Load
 */
void get_load(){
   boolean prompt,valid;  
   char recvChar;
   char bufin[12];
   int i;
  
  if (!(obd_error_flag)){                                   //if no OBD connection error
     valid=false;
     prompt=false;
     blueToothSerial.print("0104");                         //send to OBD PID command 0104 is for LOAD
     blueToothSerial.print("\r");                           //send to OBD cariage return char
     while (blueToothSerial.available() <= 0);              //wait while no data from ELM
     i=0;
     while ((blueToothSerial.available()>0) && (!prompt)){  //if there is data from ELM and prompt is false
       recvChar = blueToothSerial.read();                   //read from ELM
       if ((i<12)&&(!(recvChar==32))) {                     //the normal respond to previus command is 0104/r41 04 ??>, so count 12 chars and ignore char 32 which is space
         bufin[i]=recvChar;                                 //put received char in bufin array
         i=i+1;                                             //increase i
       }  
       if (recvChar==62) prompt=true;                       //if received char is 62 which is '>' then prompt is true, which means that ELM response is finished 
     }

    if ((bufin[5]=='4') && (bufin[6]=='1') && (bufin[7]=='0') && (bufin[8]=='4')){ //if first four chars after our command is 410D
      valid=true;                                           //then we have a correct LOAD response
    } else {
      valid=false;                                          //else we dont
    }
    if (valid){                                             //in case of correct LOAD response
      load_retries=0;                                       //reset to 0 retries
      load_error_flag=false;                                //set LOAD error flag to false
    /*
     * Calculation
     */
    String loadHex(bufin[9]); 
    String loadHex2(bufin[10]); 
    String loadHexTotal=loadHex+loadHex2;
    int DecimalDecode=hexToDec(loadHexTotal);
    load=round((float(DecimalDecode)/255)*100);                //Arredonda e devolve valor final
     }
    if (!valid){                                               //in case of incorrect LOAD response
      load_error_flag=true;                                    //set LOAD error flag to true
      load_retries+=1;                                         //add 1 retry
      load=0;                                                  //set LOAD to 0
      //Serial.println("RPM_ERROR");
      if (load_retries>=LOAD_CMD_RETRIES) obd_error_flag=true; //if retries reached LOAD_CMD_RETRIES limit then set obd error flag to true
    }    
      if ((obd_error_flag==true)){load=0; load_retries=0;}     //if no OBD connection error !GrcByte!   
  }
}
/*
 * Converting from Hex to Decimal:
 * NOTE: This function can handle a positive hex value from 0 - 65,535 (a four digit hex string).
 * For larger/longer values, change "unsigned int" to "long" in both places.
 */
unsigned int hexToDec(String hexString) {
  unsigned int decValue = 0;
  int nextInt;
  
  for (int i = 0; i < hexString.length(); i++) {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57)  nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70)  nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}
