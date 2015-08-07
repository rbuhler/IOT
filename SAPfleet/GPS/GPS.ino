  char temp = 0;
  String dataString = "";
  int count;
  int rounds = 150;

void setup(){
  Serial.begin(38400);
}
 
void loop(){
  
  get_gps();
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
