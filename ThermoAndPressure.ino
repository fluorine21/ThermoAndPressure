
#include <SPI.h>
#include "Adafruit_MAX31855.h"
//Include Teensy ADC library ( https://github.com/pedvide/ADC )
#include <ADC.h>


// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO1  23
#define MAXCS1  22
#define MAXCLK1 21
//Need to remeber to set the rest of these pins correctly
Adafruit_MAX31855 thermocouple1(MAXCLK1, MAXCS1, MAXDO1);
#define MAXDO2  3
#define MAXCS2  4
#define MAXCLK2 5
Adafruit_MAX31855 thermocouple2(MAXCLK2, MAXCS2, MAXDO2);
#define MAXDO3  7
#define MAXCS3  8
#define MAXCLK3 9
Adafruit_MAX31855 thermocouple3(MAXCLK3, MAXCS3, MAXDO3);

int halleffect = 7;
int pressure1 = A3;
int pressure2 = A5;
int NOX = A16;
int SOX = A18;//These will need to be corrected
int CO = A19;
int CO2 = A20;
int flow = A14;
int LED = 13;
int state = 0; //0 for off, 1 for on
int inbyte = 48;
String inputString = "";         // a string to hold incoming data
String result; //Holds data waiting to be pushed out over serial
boolean stringComplete = false, newData = false;  // whether the string is complete


void setup() {
  
  inputString.reserve(200);//Reserves space for the input over serial
  pinMode(halleffect, INPUT_PULLUP); //Sets up Hall Effect Sensor as input w/ a 20kohm pullup
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);
  digitalWrite(LED, HIGH);//Blinks to perform a good boot
  delay(500);
  digitalWrite(LED, LOW);
  analogReadResolution(16);//Sets analog resolution to its highest value (max 65535);
}

void loop(){
  if (Serial.available() > 0)
  {
    inbyte = Serial.read();
    newData = true;
  } 
  if(newData && inbyte == 48){
    state = 0;
    writeString("STOP");
    Serial.write('\n');
    newData = false;
    }
  else if(newData){
    state = 1;
    writeString("START");
    Serial.write('\n');
    newData = false;
    }
  if(state){
  double thermo1Total = 0; double thermo2Total = 0; double thermo3Total = 0;
  //double hallTotal = 0;
  double pressure1Total = 0; double pressure2Total = 0;
  double NOXTotal = 0; double SOXTotal = 0;
  double COTotal = 0; double CO2Total = 0;
  double flowTotal = 0;
  int i = 0;
  long time = millis();
  //Averages 5 readings to eliminate noise
  for(i = 0; i < 5; i ++){
      thermo1Total += readThermo(1);
      thermo2Total += readThermo(2);
      thermo3Total += readThermo(3);
      //hallTotal += readHall(); //Disabled for now
      pressure1Total += readPressure(1);
      pressure2Total += readPressure(2);
      NOXTotal += readNOX();
      SOXTotal += readSOX();
      COTotal += readCO();
      CO2Total += readCO2();
      flowTotal += readFlow();
    }
  //Calculates the average
  thermo1Total = thermo1Total/5.0; thermo2Total = thermo2Total/5.0; thermo3Total = thermo3Total/5.0;
  //hallTotal = hallTotal/10.0;
  pressure1Total = pressure1Total/5.0; pressure2Total = pressure2Total/5.0; 
  NOXTotal = NOXTotal/5.0; SOXTotal = SOXTotal/5.0;
  COTotal = COTotal/5.0; CO2Total = CO2Total/5.0;
  flowTotal = flowTotal/5.0; 
  result = "Temp1: " + (String)thermo1Total + " C, Temp2: " + (String)thermo2Total + " C, Temp3: " + (String)thermo3Total + " C, Flow Rate: " + flowTotal + " l/s, Pressure1: " + (String)pressure1Total + " psi, Pressure2: " + (String)pressure2Total + " psi, NOX: " + NOXTotal + " %, SOX: " + SOXTotal + " %, CO: " + COTotal + " %, CO2: " + CO2Total + " %"; 
  //Waits at least 1 second
  while(millis()-time < 1000);
  writeString(result);
  Serial.write('\n');
  //Blinks LED on readout
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  }

}
void writeString(String stringData) { // Used to serially push out a String with Serial.write()

  for (unsigned int i = 0; i < stringData.length(); i++)
  {
    Serial.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
  }

}// end writeString

//Calls whichever TC is selected and returns the temp
double readThermo(int i){
  switch (i){
    case 1:
      return thermocouple1.readCelsius();
    case 2:
      return thermocouple2.readCelsius();
    case 3:
      return thermocouple3.readCelsius();
  }
  return 0;
 
  }
double readHall(){
    long timeInitial = millis();
    while(digitalRead(halleffect)){//Waits for first pulse
      if(millis()-timeInitial > 10){
          return 0;
        }
      }
      long time1 = millis();
      timeInitial = millis();
    while(digitalRead(halleffect)){
      if(millis()-timeInitial > 10){
          return 0;
        }
      }
      long totalTime = millis() - time1;
      return (double) (1.0/(totalTime/1000))*60;
      
  }
double readPressure(int i){
  if(i == 1){
    return (((analogRead(pressure1)/65535.0)*3.31)-0.516)/.00134;
  }
  //This will need to be a different equation
    return (((analogRead(pressure2)/65535.0)*3.31)-0.516)/.00134;
  }
double readNOX(){
  return analogRead(NOX)/2.0;
  }
double readSOX(){
  return analogRead(SOX)/2.0;
  }
double readCO(){
  return analogRead(CO);
  }
double readCO2(){
  return analogRead(CO2);
  }
double readFlow(){
  return analogRead(flow)/2.0;
  }
  
  

