#include<Wire.h>
#include <SoftwareSerial.h>

// ODBII
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

// GYRO
  const int MPU=0x68;  //Endereco I2C do MPU6050
  const int roundTime=1000;
  int GyX,GyY,GyZ; //Variaveis para armazenar valores dos sensores
  
// GPS
  char temp = 0;
  String dataString = "";
  int count;
  int rounds = 150;

// General

  int wait = 1000;

void setup()
{  
// Standard
  Serial.begin(38400);
  blueToothSerial.begin(38400);
// OBDII
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);

  send_OBD_cmd("ATZ");              // send to OBD ATZ, reset
  delay(500);
  send_OBD_cmd("ATE0");             // ECHO OFF (Para o OBDII não devolver os mesmos comandos enviados)
  delay(500);

// Gyro / GPS
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
 //Inicializa o MPU-6050
  Wire.write(0); 
  Wire.endTransmission(true);
}

void loop(){
    
  get_gyro();
  get_ODB();
  get_gps();

// Gyro
  Serial.print(GyX); Serial.print(";");
  Serial.print(GyY); Serial.print(";");
  Serial.print(GyZ); Serial.print(";");
// ODBII

// GPS

// ----      
  delay(wait);

}
/*
 * Gyro
 */
    void get_gyro(){
      GyX = analogRead(A0);
      GyY = analogRead(A1);
      GyZ = analogRead(A2);
      
      gyro_clibration();
      
//      Serial.print(GyX); Serial.print(",");
//      Serial.print(GyY); Serial.print(",");
//      Serial.println(GyZ);  
      
      //Aguarda e reinicia o processo
      delay(roundTime);  
    }
  void gyro_clibration(){
    float offsetx, offsety, offsetz;
    float gainx, gainy, gainz;
    
    // Calibration Off-set for the three axes
    offsetx = -21.16; 
    offsety = -10.95;
    offsetz = -18.82;
    
    // Calibration Gain for the three axes
    gainx   = 0.0679;
    gainy   = 0.0971;
    gainz   = 0.0680;
    
    // Calibration compensation
    GyX = (GyX-offsetx) / gainx;
    GyY = (GyY-offsety) / gainy;
    GyZ = (GyZ-offsetz) / gainz;
  }

/*
 * GPS
 */
    void get_gps(){
    while(Serial.available())
    {
      count ++;
      temp = Serial.read(); 
    
      if(count >= rounds && temp == '$'){
          Serial.print(dataString);
          dataString = "";      
         count = 0;
      }
      dataString += String(temp);
    }    
  }
/*
 * ODBII
 */
  void get_ODB(){
    
    if (blueToothSerial.available()) {        //PORTA DO BLUETOOTH RX1 + TX1 (7 e 8)  
      inByte = blueToothSerial.read();
      buffer_read( inByte );   
    }else{
      
      if(i>0){Serial.print(",");};
      
      i =i+1;
      send();
    }
  }
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
    }
    if (retries>=RETRY) {                                         // if OBD cmd retries reached
      Serial.println("@ Error connecting OBD");
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
//    delay(wait);
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
