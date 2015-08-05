#include <math.h>               //Library  for funciton ROUND()
//#include <TM1638.h>          //Library for segment display
#include <SoftwareSerial.h>     //Library to make possible assign other pins as RX/TX
#include <Timer.h>              //Library to obtain data from the OBD II via 'pulse' method and not from normal delay normal (Arduino).

//TM1638 module(6, 5, 4);        //Module data pin 6, clock pin 5 and strobe pin 4
String name = "RPm OBD2";      //Para a mensagem inicial 
unsigned int rpmleds;          //holds the 8 leds values 

#define RxD 2                //Arduino pin connected to Tx of HC-05
#define TxD 3                //Arduino pin connected to Rx of HC-05

SoftwareSerial blueToothSerial(RxD,TxD); //SoftwareSerial mySerial(RX,TX); //3.3v between Arduino´s TX and module´s RX

char inByte;
boolean obd_error_flag;      //Variable for OBD connection error
#define OBD_CMD_RETRIES 3    //Number of retries for each OBD command in case of not receive prompt '>' char
/* 
 *  RPM variables
 */
boolean rpm_error_flag;      //Variable for RPM error
boolean rpm_retries;         //Variable for RPM cmd retries
unsigned int rpm;            //Variables for RPM
#define RPM_CMD_RETRIES 5    //Number of retries for RPM command
char recvCharLCD;
/* 
 *  Temperature variables
 */
boolean temp_error_flag;      //Variable for Temperatura error
boolean temp_retries;         //Variable for Temperatura cmd retries
int temp,temp_to_disp;        //Variables for Temperatura
#define TEMP_CMD_RETRIES 5    //Number of retries for Temperatura command
/* 
 *  Speed variables
 */
boolean velocidade_error_flag;      //Variable for Velocidade error
boolean velocidade_retries;         //Variable for Velocidade cmd retries
unsigned int speed;                //Variables for VelocidadePM
#define VELOCIDADE_CMD_RETRIES 5    //Number of retries for Velocidade command
/* 
 *  Load variables
 */
boolean load_error_flag;      //Variable for LoadLo error
boolean load_retries;         //Variable for Load cmd retries
int load;                     //Variables 
#define LOAD_CMD_RETRIES 5    //Number of retries for Load command
/*
 * Menu variables
 */
byte oldbuttons;                    //Display segments
int Modo=0;

byte rpmdata   = 0;            // marker that new data are available
byte tempdata  = 0;            // marker that new data are available
byte speeddata = 0;            // marker that new data are available
byte loaddata  = 0;            // marker that new data are available

Timer tRPM;
Timer tTemp;                        
Timer tSpeed;
Timer tLoad;
/*
 * SETUP
 */
void setup()
{
/*
 * Test diplsy segments/LEDs (4 red / 4 green)
 */
//  module.setLED(TM1638_COLOR_RED, 0);   // set LED number x to red
//  module.setLED(TM1638_COLOR_RED, 1);   // set LED number x to red
//  module.setLED(TM1638_COLOR_RED, 2);   // set LED number x to red
//  module.setLED(TM1638_COLOR_RED, 3);   // set LED number x to red
//  module.setLED(TM1638_COLOR_GREEN, 4); // set LED number x to green
//  module.setLED(TM1638_COLOR_GREEN, 5); // set LED number x to green
//  module.setLED(TM1638_COLOR_GREEN, 6); // set LED number x to green
//  module.setLED(TM1638_COLOR_GREEN, 7); // set LED number x to green
//  module.clearDisplay();                     // Clean segments
//  module.setupDisplay(true, 7);              // where 7 is intensity (from 0~7) - to turn on every segment AND all the LEDs, use the following
//  delay(500);
//  module.setDisplayToString(name,0,0,FONT_DEFAULT); //Mensagem Inicial
//  delay(3000);
//  module.setLEDs(0b00000000 | 0b00000000 << 8);    //Desligar todos os LEDs

// initialize both serial ports:
// Serial.begin(9600);              //Serial LCD Itead (9600)
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

//Informação inicial no Serial LCD ITEAD 16x2:    
//  Serial.print("sc;");             
//    delay(10);
//  Serial.print("sb1;");             
//    delay(10);
//  Serial.print("sd0,0;");        //ROW,COLUMN yy,xx
//    delay(10);
//  Serial.print("ssOBDII;");
//   delay(10);
//   
//Serial.print("sd1,0;");        
//delay(10);
//Serial.print("ssa iniciar...;");
//delay(1000);
//Serial.print("sc;");             
//delay(10);
//Serial.print("sd0,0;");delay(10);
//Serial.print("ssFASE;");delay(10);
//Serial.print("sd1,0;");delay(10);
//Serial.print("ss1/2;");delay(10);
  
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

//Menu inicial:
//  Serial.print("sc;");            
//    delay(10);
//  Serial.print("sd0,0;");        //ROW,COLUMN: yy,xx
//    delay(10);
//  Serial.print("ssEscolha o MODO:;"); 
//   delay(50);
//  Serial.print("sd1,0;");        //ROW,COLUMN: yy,xx
//   delay(50);
//  Serial.print("ss1,2,3,4;"); 
//   delay(50); 
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
       recvCharLCD+=recvChar;
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
 
 
//void SerialLcdDisplay(String Mode, String Valor)
//{
//  Serial.print("sc;");            
//    delay(10);
//  Serial.print("sd0,0;");        //ROW,COLUMN: yy,xx
//    delay(10);
//  Serial.print("ss"+ Mode + ";"); 
//   delay(50);
//  Serial.print("sd1,10;");        //ROW,COLUMN: yy,xx
//   delay(50);
//  Serial.print("ss"+ Valor + ";"); 
//   delay(50);
//}

void loop() {
  
//byte buttons=module.getButtons();
// 
//if (buttons != 0) {
//  if (buttons != oldbuttons) {
//      oldbuttons = buttons;
//
// switch(buttons)
// {
//case 1:
////Serial.println("BOTAO 1"); delay(50);
//Modo=1;
//module.clearDisplay(); //CLEAR
//SerialLcdDisplay("MODO1 - RPM", 0);
// break;
// case 2:
////Serial.println("BOTAO 2"); delay(50);
//Modo=2;
//module.clearDisplay(); //CLEAR
//SerialLcdDisplay("MODO2 - TEMP", "C");
// break;
// case 4:
////Serial.println("BOTAO 3"); delay(50);
//Modo=3;
//module.clearDisplay(); //CLEAR
//SerialLcdDisplay("MODO3 - VELOC.", "Km");
// break;
// case 8:
////Serial.println("BOTAO 4"); delay(50);
//Modo=4;
//module.clearDisplay(); //CLEAR
//SerialLcdDisplay("MODO4 - LOAD", "%");
// break;
// case 16:
////Serial.println("BOTAO 5"); delay(50); //NÃO USADO
// break;
// case 32:
////Serial.println("BOTAO 6"); delay(50); //NÃO USADO
// break;
// case 64: 
////Serial.println("BOTAO 7"); delay(50); //NÃO USADO
// break;
// case 128:
////Serial.println("BOTAO 8"); delay(50); //NÃO USADO
// break;
// }
//} }  
  
  
if (Modo==1) { //RPM
tRPM.update();  //update of timer for calling rpm_calc
if (rpmdata!=rpm){
    rpmdata=rpm;
    
//   rpmleds = map(rpm,950,2600,0,8);    
//   module.setDisplayToDecNumber(rpm, 0, false); // RPM ATUAL NO DISPLAY DE SEGMENTOS
//
//   if (rpmleds==0){            
//     module.setLEDs(0b00000000 | 0b00000000 << 8); //Todos desligados
//   }
//   if (rpmleds==1){
//     module.setLEDs(0b00000000 | 0b00000001<< 8 ); //Da Direita para a esquerda no código e da esquerda para a direita no Display| 1º led da esquerda VERDE   
//   }
//   if (rpmleds==2){
//     module.setLEDs(0b00000000 | 0b00000011<< 8 ); // 2 leds da esquerda VERDE  
//   }
//   if (rpmleds==3){
//     module.setLEDs(0b00000000 | 0b00000111<< 8 ); // 3 leds da esquerda VERDE  
//   }
//   if (rpmleds==4){
//     module.setLEDs(0b00000000 | 0b00001111<< 8); // 4 leds da esquerda VERDE  
//   }
//   if (rpmleds==5){
//     module.setLEDs(0b00000000 | 0b00011111 << 8); // 5 leds da esquerda VERDE  
//   }
//   if (rpmleds==6){
//     module.setLEDs(0b00100000 | 0b00011111<< 8 ); // 5 leds da esquerda VERDE + 1 VERMELHO  
//   }
//   if (rpmleds==7){
//     module.setLEDs(0b01100000 | 0b00011111<<8 ); // 5 leds da esquerda VERDE + 2 VERMELHO  
//   }
//   if (rpmleds==8){
//     //module.setLEDs(0b11100000 | 0b000011111<<8 ); // 5 leds da esquerda VERDE + 3 VERMELHO  
//     module.setLEDs(0b11111111 | 0b111111111<<8 ); //TODOS OS LEDS LIGADOS a VERDE + VERMELHO
//      }  
    } 
  }

if (Modo==2) { //TEMPERATURA
tTemp.update();  //update of timer for calling temperatura_calc

if (tempdata!=temp){
    tempdata=temp;
//   rpmleds = map(temp,10,200,0,8);    // Colocar potênciometro para definir valor |distributes the TEMPERATURA level to the 8 leds 
//   module.setDisplayToDecNumber(temp, 0, false); // TEMPERATURA ATUAL NO DISPLAY DE SEGMENTOS
//   //ou---> module.setDisplayToString (String(temp) + " C", 0, false); 
//    
//   if (rpmleds==0){            
//     module.setLEDs(0b00000000 | 0b00000000 << 8); //Todos desligados
//   }
//   if (rpmleds==1){
//     module.setLEDs(0b00000000 | 0b00000001<< 8 ); //Da Direita para a esquerda no código e da esquerda para a direita no Display| 1º led da esquerda VERDE   
//   }
//   if (rpmleds==2){
//     module.setLEDs(0b00000000 | 0b00000011<< 8 ); // 2 leds da esquerda VERDE  
//   }
//   if (rpmleds==3){
//     module.setLEDs(0b00000000 | 0b00000111<< 8 ); // 3 leds da esquerda VERDE  
//   }
//   if (rpmleds==4){
//     module.setLEDs(0b00000000 | 0b00001111<< 8); // 4 leds da esquerda VERDE  
//   }
//   if (rpmleds==5){
//     module.setLEDs(0b00000000 | 0b00011111 << 8); // 5 leds da esquerda VERDE  
//   }
//   if (rpmleds==6){
//     module.setLEDs(0b00100000 | 0b00011111<< 8 ); // 5 leds da esquerda VERDE + 1 VERMELHO  
//   }
//   if (rpmleds==7){
//     module.setLEDs(0b01100000 | 0b00011111<<8 ); // 5 leds da esquerda VERDE + 2 VERMELHO  
//   }
//   if (rpmleds==8){
//     //module.setLEDs(0b11100000 | 0b000011111<<8 ); // 5 leds da esquerda VERDE + 3 VERMELHO  
//     module.setLEDs(0b11111111 | 0b111111111<<8 ); //TODOS OS LEDS LIGADOS a VERDE + VERMELHO
//     } 
    } 
  }

if (Modo==3) {            // Speed
tSpeed.update();          // update of timer for calling velocidade_calc

if (speeddata!=speed){
    speeddata=speed;
      
//   rpmleds = map(velocidade,10,210,0,8);    // Colocar potênciometro para definir valor |distributes the VELOCIDADE level to the 8 leds 
//
//    module.setDisplayToDecNumber(velocidade, 0, false); // RPM ATUAL NO DISPLAY DE SEGMENTOS
//
//   if (rpmleds==0){            
//     module.setLEDs(0b00000000 | 0b00000000 << 8); //Todos desligados
//   }
//   if (rpmleds==1){
//     module.setLEDs(0b00000000 | 0b00000001<< 8 ); //Da Direita para a esquerda no código e da esquerda para a direita no Display| 1º led da esquerda VERDE   
//   }
//   if (rpmleds==2){
//     module.setLEDs(0b00000000 | 0b00000011<< 8 ); // 2 leds da esquerda VERDE  
//   }
//   if (rpmleds==3){
//     module.setLEDs(0b00000000 | 0b00000111<< 8 ); // 3 leds da esquerda VERDE  
//   }
//   if (rpmleds==4){
//     module.setLEDs(0b00000000 | 0b00001111<< 8); // 4 leds da esquerda VERDE  
//   }
//   if (rpmleds==5){
//     module.setLEDs(0b00000000 | 0b00011111 << 8); // 5 leds da esquerda VERDE  
//   }
//   if (rpmleds==6){
//     module.setLEDs(0b00100000 | 0b00011111<< 8 ); // 5 leds da esquerda VERDE + 1 VERMELHO  
//   }
//   if (rpmleds==7){
//     module.setLEDs(0b01100000 | 0b00011111<<8 ); // 5 leds da esquerda VERDE + 2 VERMELHO  
//   }
//   if (rpmleds==8){
//     //module.setLEDs(0b11100000 | 0b000011111<<8 ); // 5 leds da esquerda VERDE + 3 VERMELHO  
//     module.setLEDs(0b11111111 | 0b111111111<<8 );  //TODOS OS LEDS LIGADOS a VERDE + VERMELHO
//     } 
    } 
  }

if (Modo==4) {            //ENGINE LOAD %
tLoad.update();           //update of timer for calling load_calc

if (loaddata!=load){
    loaddata=load;
      
//   rpmleds = map(load,10,100,0,8);    // Colocar potênciometro para definir valor |distributes the LOAD level to the 8 leds 
//   module.setDisplayToDecNumber(load, 0, false); 
//   if (rpmleds==0){            
//     module.setLEDs(0b00000000 | 0b00000000 << 8); //Todos desligados
//   }
//   if (rpmleds==1){
//     module.setLEDs(0b00000000 | 0b00000001<< 8 ); //Da Direita para a esquerda no código e da esquerda para a direita no Display| 1º led da esquerda VERDE   
//   }
//   if (rpmleds==2){
//     module.setLEDs(0b00000000 | 0b00000011<< 8 ); // 2 leds da esquerda VERDE  
//   }
//   if (rpmleds==3){
//     module.setLEDs(0b00000000 | 0b00000111<< 8 ); // 3 leds da esquerda VERDE  
//   }
//   if (rpmleds==4){
//     module.setLEDs(0b00000000 | 0b00001111<< 8); // 4 leds da esquerda VERDE  
//   }
//   if (rpmleds==5){
//     module.setLEDs(0b00000000 | 0b00011111 << 8); // 5 leds da esquerda VERDE  
//   }
//   if (rpmleds==6){
//     module.setLEDs(0b00100000 | 0b00011111<< 8 ); // 5 leds da esquerda VERDE + 1 VERMELHO  
//   }
//   if (rpmleds==7){
//     module.setLEDs(0b01100000 | 0b00011111<<8 ); // 5 leds da esquerda VERDE + 2 VERMELHO  
//   }
//   if (rpmleds==8){
//     //module.setLEDs(0b11100000 | 0b000011111<<8 ); // 5 leds da esquerda VERDE + 3 VERMELHO  
//     module.setLEDs(0b11111111 | 0b111111111<<8 );  //TODOS OS LEDS LIGADOS a VERDE + VERMELHO
//    } 
    } 
  }
}
// ---------------------------------------------------------------------------------------------------

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
       if ((i<15)&&(!(recvChar==32))) {                     //the normal respond to previus command is 010C1/r41 0C ?? ??>, so count 15 chars and ignore char 32 which is space | GRCBYTE: ele ignora o "/r"
         bufin[i]=recvChar;                                 //put received char in bufin array
         i=i+1;                                             //increase i
       }  
       if (recvChar==62) prompt=true;                       //if received char is 62 which is '>' then prompt is true, which means that ELM response is finished 
     }

    if ((bufin[6]=='4') && (bufin[7]=='1') && (bufin[8]=='0') && (bufin[9]=='C')){ //if first four chars after our command is 410C
      valid=true;                                                                  //then we have a correct RPM response
    } else {
      valid=false;                                                                 //else we dont
    }
    if (valid){                                                                    //in case of correct RPM response
      rpm_retries=0;                                                               //reset to 0 retries
      rpm_error_flag=false;    
/* Start calculation of real RPM value
   RPM is coming from OBD in two 8bit(bytes) hex numbers for example A=0B and B=6C
   the equation is ((A * 256) + B) / 4, 
   so 0B=11 and 6C=108 so rpm=((11 * 256) + 108) / 4 = 731 a normal idle car engine rpm */
      rpm=0;                                                                                      
      for (i=10;i<14;i++){                        //in that 4 chars of bufin array which is the RPM value
        if ((bufin[i]>='A') && (bufin[i]<='F')){  //if char is between 'A' and 'F'
          bufin[i]-=55;                           //'A' is int 65 minus 55 gives 10 which is int value for hex A
        }  
        if ((bufin[i]>='0') && (bufin[i]<='9')){  //if char is between '0' and '9'
          bufin[i]-=48;                           //'0' is int 48 minus 48 gives 0 same as hex
        }
        rpm=(rpm << 4) | (bufin[i] & 0xf);        //shift left rpm 4 bits and add the 4 bits of new char
      }
      rpm=rpm >> 2;                               //finaly shift right rpm 2 bits, rpm=rpm/4
    }
      
    }
    if (!valid){                                  //in case of incorrect RPM response
      rpm_error_flag=true;                        //set rpm error flag to true
      rpm_retries+=1;                             //add 1 retry
      rpm=0;     
      //set rpm to 0
      //Serial.println("RPM_ERROR");
      if (rpm_retries>=RPM_CMD_RETRIES) obd_error_flag=true;  //if retries reached RPM_CMD_RETRIES limit then set obd error flag to true
    }
    
      if ((obd_error_flag==true)){rpm=0; rpm_retries=0;} //if no OBD connection error !GrcByte!
} // RPM

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
    temp=hexToDec(tempHexTotal);
    temp-=40; 
     }
    if (!valid){                                           //in case of incorrect TEMP response
      temp_error_flag=true;                                //set rpm error flag to true
      temp_retries+=1;                                     //add 1 retry
      temp=0;                                              //set temperatura to 0

      //Serial.println("RPM_ERROR");
      if (temp_retries>=TEMP_CMD_RETRIES) obd_error_flag=true;  //if retries reached TEMP_CMD_RETRIES limit then set obd error flag to true
    }    
      if ((obd_error_flag==true)){temp=0; temp_retries=0;}     //if no OBD connection error !GrcByte!   
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
     blueToothSerial.print("010D");                        //send to OBD PID command 010D is for VELOCIDADE
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
      valid=true;                                                                  //then we have a correct VELOCIDADE response
    } else {
      valid=false;                                                                 //else we dont
    }
    if (valid){                                                                    //in case of correct VELOCIDADE response
      velocidade_retries=0;                                                               //reset to 0 retries
      velocidade_error_flag=false;                                                        //set rpm error flag to false
      
    /* 
     * Calculation 
     */
    String velocidadeHex(bufin[9]); 
    String velocidadeHex2(bufin[10]); 
    String velocidadeHexTotal=velocidadeHex+velocidadeHex2;
    speed=hexToDec(velocidadeHexTotal);
     }
    if (!valid){                                             // in case of incorrect VELOCIDADE response
      velocidade_error_flag=true;                            // set rpm error flag to true
      velocidade_retries+=1;                                 // add 1 retry
      speed=0;                                               // set speed to 0
      //Serial.println("RPM_ERROR");
      if (velocidade_retries>=VELOCIDADE_CMD_RETRIES) obd_error_flag=true;  //if retries reached VELOCIDADE_CMD_RETRIES limit then set obd error flag to true
    }    
      if ((obd_error_flag==true)){speed=0; velocidade_retries=0;} //if no OBD connection error !GrcByte!
   
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
