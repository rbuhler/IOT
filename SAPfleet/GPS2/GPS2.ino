  char temp = 0;
  String dataString = "";
  int count;
  int rounds = 150;

  String csv[3];

void setup(){
  Serial.begin(38400);
}
 
void loop(){

  get_gps();  
  csv[2].trim();
  while (csv[2].length() == 0){
    get_gps();
  }
  csv[2].trim();
  Serial.println("");
  Serial.println(csv[2]);        

  delay(2000);
  
}
  void get_gps(){
    while(Serial.available())
    {
      
      temp = Serial.read();
    
      if(count >= rounds && temp == '$'){
          
        dataString.trim();
        csv[2] = dataString;
        dataString = "";      
        count = 0;
      }
         
         if (temp == 10 || temp == 13 || temp == 32){
          
         }else{
            count ++;
            dataString += String(temp);
         }
    }    
  }
