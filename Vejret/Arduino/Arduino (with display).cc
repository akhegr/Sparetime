//TMP36 Pin Variables
int sensorPin = A1; //the analog pin the TMP36's Vout (sense) pin is connected to
                   //the resolution is 10 mV / degree centigrade with a
                   //500 mV offset to allow for negative temperatures

//Rainsensor
int rainPin = A0;
int thresholdValue = 500; // you can adjust the threshold value


// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


 

byte newChar1[8] = { //ø
  B00000,
  B01101,
  B10010,
  B10101,
  B01001,
  B10110,
  B00000,
  B00000
};

void setup()
{
  Serial.begin(9600);  //Start the serial connection with the computer
                       //to view the result open the serial monitor 

  pinMode(sensorPin, INPUT);  //evt. fjern
  pinMode(rainPin, INPUT);

  lcd.begin(16, 2);
  //lcd.print("hello, world!");


  lcd.createChar(1, newChar1);  
}
 
void loop()                     // run over and over again
{
  lcd.clear();
  
 //getting the voltage reading from the temperature sensor
 int reading = analogRead(sensorPin);  
 
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage = reading * 5.0;
 voltage /= 1024.0; 
 
        // print out the voltage
Serial.print(voltage);
Serial.println(" volt");
 
 // now print out the temperature
 float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
 Serial.print(temperatureC);
 Serial.println(" grader C");

   
   lcd.setCursor(0, 0);
   lcd.print(temperatureC);
   lcd.print(" ");
   lcd.print((char)223);
   lcd.print("C");// grader C
   //lcd.print(4); 
  
  // read the input on rainsensor pin:
  int sensorValue = analogRead(rainPin);
  Serial.print(sensorValue);
  
  lcd.setCursor(0, 1);
  //lcd.print(sensorValue);

  if(sensorValue < thresholdValue){
    Serial.println(" - It's wet");
    lcd.print("Der regner");
  } else {
    Serial.println(" - It's dry");
    lcd.print("Der er t");
    lcd.write(1);
    lcd.print("rt");
  }
  Serial.println();
  delay(1000);   //waiting in 1 second
}
