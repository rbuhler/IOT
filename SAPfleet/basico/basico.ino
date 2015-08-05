  int  index = 0;
  String string;
  char content = '@';

void setup()
{
  Serial.begin(9600);
}

void loop(){
  
  while (index <= 10){

    index ++;
    string += String(content);
  }

  index = 0;
  Serial.print("Result : ");
  Serial.println(string);
  string = "";

  delay(1000);
  
  
  }
