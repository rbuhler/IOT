//Carrega a biblioteca Wire
#include<Wire.h>

const int MPU=0x68;  //Endereco I2C do MPU6050
const int roundTime=1000;

int AcX,AcY,AcZ; //Variaveis para armazenar valores dos sensores

void setup()
{  
  Serial.begin(9600);

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
   
  //Inicializa o MPU-6050
  Wire.write(0); 
  Wire.endTransmission(true);
     
}
void loop()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  //Solicita os dados do sensor
  Wire.requestFrom(MPU,14,true);  
  
  //Armazena o valor dos sensores nas variaveis correspondentes
  AcX=Wire.read()<<8|Wire.read();  //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
   
  //Envia valor X do acelerometro para a serial e o LCD
  Serial.print(AcX);Serial.print(",");
  //Envia valor Y do acelerometro para a serial e o LCD
  Serial.print(AcY);Serial.print(",");
  //Envia valor Z do acelerometro para a serial e o LCD
  Serial.println(AcZ);
  //Aguarda e reinicia o processo
  delay(roundTime);
}
