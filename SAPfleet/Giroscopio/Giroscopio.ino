// GYRO
  const int roundTime=1000;
  int GyX,GyY,GyZ; //Variaveis para armazenar valores dos sensores
  
void setup()
{  
  Serial.begin(38400);   
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
