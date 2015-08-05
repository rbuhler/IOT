#include<Wire.h>

// GYRO
  const int MPU=0x68;  //Endereco I2C do MPU6050
  const int roundTime=1000;
  int GyX,GyY,GyZ; //Variaveis para armazenar valores dos sensores
  
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
  GyX = analogRead(A0);
  GyY = analogRead(A1);
  GyZ = analogRead(A2);
  
  gyro_clibration();
  
  Serial.print(GyX); Serial.print(",");
  Serial.print(GyY); Serial.print(",");
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
