#include<Wire.h>

// GYRO
  const int MPU=0x68;  //Endereco I2C do MPU6050
  const int roundTime=1000;
  int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; //Variaveis para armazenar valores dos sensores
// GPS
  char temp = 0;
  String dataString = "";
  int count;
  int rounds = 150;

void setup()
{  
  Serial.begin(38400);

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
  Tmp=Wire.read()<<8|Wire.read();  //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  gyro_clibration();
  
  Serial.print(AcX);             Serial.print(",");
  Serial.print(AcY);             Serial.print(",");
  Serial.print(AcZ);             Serial.print(",");
  Serial.print(Tmp/340.00+36.53);Serial.print(",");
  Serial.print(GyX);             Serial.print(",");
  Serial.print(GyY);             Serial.print(",");
  Serial.println(GyZ);  
  
  //Aguarda e reinicia o processo
  delay(roundTime);
  get_gps();
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

    void get_gps(){
    while(Serial.available())
    {
      count ++;
      temp = Serial.read();
    
      if(count >= rounds && temp == '$'){
          Serial.println(dataString);  
          dataString = "";      
         count = 0;
      }
      dataString += String(temp);
    }    
  }
