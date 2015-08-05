//Carrega a biblioteca Wire
#include<Wire.h>

const int MPU=0x68;  //Endereco I2C do MPU6050
const int roundTime=1000;

int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; //Variaveis para armazenar valores dos sensores
int AcXold,AcYold,AcZold,Tmpold,GyXold,GyYold,GyZold; //Variaveis para armazenar valores dos sensores

int AcXz,AcYz,AcZz,GyXz,GyYz,GyZz; // Zero
int run_zero = 0;
int variance = 200;

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
  // Zero
  if(run_zero<=1){
    AcXz=Wire.read()<<8|Wire.read();  //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
    AcYz=Wire.read()<<8|Wire.read();  //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZz=Wire.read()<<8|Wire.read();  //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();   //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyXz=Wire.read()<<8|Wire.read();  //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyYz=Wire.read()<<8|Wire.read();  //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZz=Wire.read()<<8|Wire.read();  //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

    Serial.println(AcXz);
    Serial.println(AcYz);
    Serial.println(AcZz);
    Serial.println(GyXz);
    Serial.println(GyYz);
    Serial.println(GyZz);
    
    run_zero ++;
   
  }else{
    //Armazena o valor dos sensores nas variaveis correspondentes
    AcX=Wire.read()<<8|Wire.read();  //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
    AcY=Wire.read()<<8|Wire.read();  //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();  //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read();  //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read();  //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read();  //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
     
    // Calibration from the first read
    AcX=AcX-AcXz;
    AcY=AcY-AcYz;
    AcZ=AcZ-AcZz;
    GyX=GyX-GyXz;
    GyY=GyY-GyYz;
    GyZ=GyZ-GyZz;
    
    // Check the variance
    if((AcX - AcXold)<=100){ AcX = AcXold; }
    if((AcY - AcYold)<=100){ AcY = AcYold; }
    if((AcZ - AcZold)<=100){ AcZ = AcZold; }

    if((GyX - GyXold)<=100){ GyX = GyXold; }
    if((GyY - GyYold)<=100){ GyY = GyYold; }
    if((GyZ - GyZold)<=100){ GyZ = GyZold; }

    //Envia valor X do acelerometro para a serial e o LCD
    Serial.print(AcX);Serial.print(",");
    //Envia valor Y do acelerometro para a serial e o LCD
    Serial.print(AcY);Serial.print(",");
    //Envia valor Z do acelerometro para a serial e o LCD
    Serial.print(AcZ);Serial.print(",");
    //Calcula a temperatura em graus Celsius
    Serial.print(Tmp/340.00+36.53);Serial.print(",");
    //Envia valor X do giroscopio para a serial e o LCD
    Serial.print(GyX);Serial.print(",");
    //Envia valor Y do giroscopio para a serial e o LCD  
    Serial.print(GyY);Serial.print(",");
    //Envia valor Z do giroscopio para a serial e o LCD
    Serial.println(GyZ);  
  }
    // Current reported values
    AcXold=AcX;
    AcYold=AcY;
    AcZold=AcZ;
    GyXold=GyX;
    GyYold=GyY;
    GyZold=GyZ;
    // 
  //Aguarda e reinicia o processo
  delay(roundTime);
}
