#include <AFMotor.h>
#include <Servo.h> 

AF_DCMotor motor1(1); // create motor #2, 64KHz pwm
AF_DCMotor motor2(2); // create motor #2, 64KHz pwm
AF_DCMotor motor3(3); // create motor #2, 64KHz pwm
AF_DCMotor motor4(4); // create motor #2, 64KHz pwm
String readString;

int drejekransRight = 14;
int drejekransLeft = 15;
int kloOpen = 16;
int kloClose = 17;

int speedMa = 128; //frem+tilbage
int speedMb = 128; //til siden

void setup() {     // presenting pins as outputs
  Serial.begin(9600); // initialization

  //motor1.setSpeed(128);  //bliver indstillet fra appen (speedMa og b)
  //motor2.setSpeed(128);  //bliver indstillet fra appen (speedMa og b)
  motor3.setSpeed(255);
  motor4.setSpeed(255);
    
  pinMode(drejekransRight, OUTPUT);
  pinMode(drejekransLeft, OUTPUT);
  pinMode(kloOpen, OUTPUT);
  pinMode(kloClose, OUTPUT);
}

void loop() {
  while(Serial.available()){ // the code here is for bluetooth
    delay(3);
    char c = Serial.read();
    readString += c;
   
   //int speed = Serial.parseInt(); 
    //int 1speed = Serial.parseInt(); 
    //int 2speed = Serial.parseInt();   
}
  
  if(readString.length() > 0){ // when a certain button is pressed the reaction will be...
    Serial.println(readString);
    
    //Larvebånd styring    
    if(readString == "M12U"){  //Robotten kører frem
      motor1.setSpeed(speedMa);
      motor2.setSpeed(speedMa);
      
      motor1.run(FORWARD);
      motor2.run(FORWARD);
    }
    else if(readString == "M12D"){  //Robotten kører bagud
      motor1.setSpeed(speedMa);
      motor2.setSpeed(speedMa);
      
      motor1.run(BACKWARD);
      motor2.run(BACKWARD);
    }
    else if(readString == "M12L"){  //Robotten kører mod venstre
      motor1.setSpeed(speedMb);
      motor2.setSpeed(speedMb);
      
      motor1.run(FORWARD);
      motor2.run(BACKWARD);
    }
    else if(readString == "M12R"){  //Robotten kører mod højre
      motor1.setSpeed(speedMa);
      motor2.setSpeed(speedMa);
      
      motor1.run(BACKWARD);
      motor2.run(FORWARD);
    }
    else if(readString == "M12STOP"){  //Robotten stopper
      motor1.run(RELEASE);
      motor2.run(RELEASE);
    }
    
    //Inderste arm styring   
    else if(readString == "M3L"){
      motor3.run(FORWARD);
    }
    else if(readString == "M3R"){
      motor3.run(BACKWARD);
    }
    
    //Yderste arm styring   
    else if(readString == "M4L"){
      motor4.run(FORWARD);
    }
    else if(readString == "M4R"){
      motor4.run(BACKWARD);
    }

    /*
     * Drejekrans og klo sendes til Arduino 2
     */
     
    //Drejekrans styring   
    else if(readString == "M5L"){
      digitalWrite(drejekransLeft, HIGH);
      digitalWrite(drejekransRight, LOW);
    }
    else if(readString == "M5R"){
      digitalWrite(drejekransRight, HIGH);
      digitalWrite(drejekransLeft, LOW);
    }
    
  //Klo styring    
    else if(readString == "M6L"){
      digitalWrite(kloOpen, HIGH);
      digitalWrite(kloClose, LOW);      
    }
    else if(readString == "M6R"){
      digitalWrite(kloClose, HIGH);
      digitalWrite(kloOpen, LOW);
    }    
    
    
  //Stop    
    else if(readString == "STOP"){
      motor1.run(RELEASE);
      motor2.run(RELEASE);
      motor3.run(RELEASE);
      motor4.run(RELEASE);
      digitalWrite(drejekransLeft, LOW);
      digitalWrite(drejekransRight, LOW);
      digitalWrite(kloOpen, LOW);
      digitalWrite(kloClose, LOW);  
    }    
    
    
    
  //Indstil motorfart    
    else{
      if (readString.startsWith("M1Speed"))
      {
        
      }
       else if (readString.startsWith("M1Speed"))
      {
        
      }
    }
  }
  readString="";
}
