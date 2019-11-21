String readString;

int relay1 = 7;
int relay2 = 6;

void setup() {

  pinMode(relay1, OUTPUT);
  //digitalWrite(relay1, LOW);
  
  pinMode(relay2, OUTPUT);
  //digitalWrite(relay2, LOW);

  Serial.begin(9600); // Default connection rate for the BT module
}

void loop() {
  while(Serial.available()){ // the code here is for bluetooth
    delay(3);
    char c = Serial.read();
    readString += c;
  }
  if(readString.length() > 0){ // when a certain button is pressed the reaction will be...
    Serial.println(readString);
    
    if(readString == "left"){
      Serial.println("Port 1 - left");  
      digitalWrite(relay1,HIGH);
      delay(1000);
      digitalWrite(relay1,LOW);  
    }
    else if(readString == "right"){
      Serial.println("Port 2 - right");
      digitalWrite(relay2,HIGH);
      delay(1000);
      digitalWrite(relay2,LOW);
    } else{
      Serial.write("An error occured!");
    }
  }
  readString="";
}
