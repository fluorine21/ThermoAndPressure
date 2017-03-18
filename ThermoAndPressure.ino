

#include "Adafruit_MAX31855.h"
//Include Teensy ADC library ( https://github.com/pedvide/ADC )
#include <ADC.h>

#define HWSERIAL Serial1

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO3  23
#define MAXCS3  21
#define MAXCLK3 20
//Need to remeber to set the rest of these pins correctly

#define MAXDO2  1 
#define MAXCS2  2
#define MAXCLK2 3

#define MAXDO1  5
#define MAXCS1  6
#define MAXCLK1 7

Adafruit_MAX31855 thermocouple1(MAXCLK1, MAXCS1, MAXDO1);
Adafruit_MAX31855 thermocouple2(MAXCLK2, MAXCS2, MAXDO2);
Adafruit_MAX31855 thermocouple3(MAXCLK3, MAXCS3, MAXDO3);

double avgs = 5; //Specifies the number of averadges to be taken when recording data
int halleffect = 7;
int pressure1 = A3;
int pressure2 = A5;
int NOX = A14;
int SOX = A15;//These will need to be corrected
int CO = A16;
int CO2 = A17;
int O2 = A18;
int flow = A20;
int UFPM = A22;

int LED = 13;
int state = 0; //0 for off, 1 for on
int inbyte = 48;
String inputString = "";         // a string to hold incoming data
String result; //Holds data waiting to be pushed out over serial
boolean stringComplete = false, newData = false;  // whether the string is complete

double indexer = 0;

double flowCorrectionValue = 183; //(l/s)/V
double pressure1CorrectionValue = 0.00134; // V/psi
double pressure2CorrectionValue = 0.00148; // V/psi
double NOXCorrectionFactor = 250;
double SOXCorrectionFactor = 200;
double COCorrectionFactor = 500;
double CO2CorrectionFactor = 20;
double O2CorrectionFactor = 25;
double UFPMCorrectionFactor = 1;

void setup() {
  
  inputString.reserve(200);//Reserves space for the input over serial
  pinMode(halleffect, INPUT_PULLUP); //Sets up Hall Effect Sensor as input w/ a 20kohm pullup
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);
  HWSERIAL.begin(9600);
  
  digitalWrite(LED, HIGH);//Blinks to perform a good boot
  delay(500);
  digitalWrite(LED, LOW);
  analogReadResolution(16);//Sets analog resolution to its highest value (max 65535);
  indexer = 0;

  
}

void loop(){
  if (Serial.available() > 0)
  {
    inbyte = Serial.read();
    newData = true;
  } 
  if(newData && inbyte == 48){
    state = 0;
    Serial.println("STOP");
    newData = false;
    indexer = 0;
    }
  else if(newData){
    state = 1;
    Serial.println("START");
    result = "Data,Temp1,Temp2,Temp3,Flow Rate (l/s),Pressure1 (psi),Pressure2 (psi),NOX (ppm),SOX (ppm),CO(ppm),CO2 (%),O2 (%),UFM (p/cm^3 x 10^6)" ;
    Serial.println(result);
    newData = false;
    indexer = 0;
    }
  if(state){
  double thermo1Total = 0; double thermo2Total = 0; double thermo3Total = 0;
  //double hallTotal = 0;
  double pressure1Total = 0; double pressure2Total = 0;
  double NOXTotal = 0; double SOXTotal = 0;
  double COTotal = 0; double CO2Total = 0;
  double O2Total = 0;
  double UFPMTotal = 0;
  double flowTotal = 0;
  int i = 0;
  long time = millis();
  //Averages readings to eliminate noise
  for(i = 0; i < (int)avgs; i ++){
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
      O2Total += readO2();
      UFPMTotal += readUFPM();
    }
  //Calculates the average
  thermo1Total = thermo1Total/avgs; thermo2Total = thermo2Total/avgs; thermo3Total = thermo3Total/avgs;
  //hallTotal = hallTotal/10.0;
  pressure1Total = pressure1Total/avgs; pressure2Total = pressure2Total/avgs; 
  NOXTotal = NOXTotal/avgs; SOXTotal = SOXTotal/avgs;
  COTotal = COTotal/avgs; CO2Total = CO2Total/avgs;
  O2Total = O2Total/avgs;
  UFPMTotal = UFPMTotal/avgs;
  flowTotal = flowTotal/avgs; 
  result = (String)(int)indexer + "," + (String)thermo1Total + "," + (String)thermo2Total + "," + (String)thermo3Total + "," + flowTotal + "," + (String)pressure1Total + "," + (String)pressure2Total + "," + (String)NOXTotal + "," + (String)SOXTotal + "," + (String)COTotal + "," + (String)CO2Total + "," + (String)O2Total + "," + (String)UFPMTotal;
  //Waits at least 1 second
  while(millis()-time < 1000);
  Serial.println(result);
  //Blinks LED on readout
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  }
  indexer++;

}

//Calls whichever TC is selected and returns the temp
double readThermo(int i){
  delay(2);
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

 //These all convert an analog reading to volts before multiplying it by the appropriate scaling factor
double readPressure(int i){
  delay(2);
  if(i == 1){
    return (((analogRead(pressure1)/65535.0)*3.31)-0.518)/pressure1CorrectionValue;
  }
  //This will need to be a different equation
    return (((analogRead(pressure2)/65535.0)*3.31)-0.508)/pressure2CorrectionValue;
  }
double readNOX(){
  delay(2);
  return ((analogRead(NOX)/65535.0)*3.31)*NOXCorrectionFactor;
  }
double readSOX(){
  delay(2);
  return ((analogRead(SOX)/65535.0)*3.31)*SOXCorrectionFactor;
  }
double readCO(){
  delay(2);
  return ((analogRead(CO)/65535.0)*3.31)*COCorrectionFactor;
  }
double readCO2(){
  delay(2);
  return ((analogRead(CO2)/65535.0)*3.31)*CO2CorrectionFactor;
  }
double readFlow(){
  delay(2);
  return ((analogRead(flow)/65535.0)*3.31)*flowCorrectionValue;
  }
double readO2(){
  delay(2);
  return ((analogRead(O2)/65535.0)*3.31)*O2CorrectionFactor;
  }
double readUFPM(){
  delay(2);
  //3.05 accounts for the internal voltage divider
  return ((analogRead(UFPM)/65535.0)*3.31)*3.0588*UFPMCorrectionFactor;
  }
  

