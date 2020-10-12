#include <WiFi.h>                     //wifi connection libraries
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal.h>
#include <driver/dac.h>
#include <string>

LiquidCrystal lcd(15,16,17,18,19,21);

#define BLYNK_AUTH        "iPYKltlmtuP0iVfLA4eZK9n3JCzXNuDm"
#define WIFI_SSID         "This is Vlag"//"HappyAt"
#define WIFI_PASS         "kancrpa802333"//"HappyLife58"
#define LCD_LEN            16

#define TEMP_PIN        35       //analog input pin constant<br> was 2 and set to 35
#define ANALOG_FAN_PIN  DAC_CHANNEL_1      //variable to use if using dac functions
#define DIGITAL_FAN_PIN 25                //variable to use if using digital functions
#define POT_PIN         34

#define OFF_VOLTAGE     0
#define LOW_VOLTAGE     85
#define MEDIUM_VOLTAGE  170
#define HIGH_VOLTAGE    255

#define OFF_STATE       0
#define AUTO_STATE      1
#define MANUAL_STATE    2

BlynkTimer fanUpdateTimer;

int tempVal;    // temperature sensor raw readings
float volts;    // variable for storing voltage
float tempC;     // actual temperature variable
float tempF;
int potValue = 0; // variable for storing the potentiometer value

int desiredTemp;
int fakeTemp;
int manualFanPower;
int currentFanPower;
int previousFanPower;

bool systemOn;
bool manualMode;

int previousState;
int currentState;
char line0[LCD_LEN];
char line1[LCD_LEN];




void fanUpdateEvent() {//No.2
  currentFanPower = OFF_VOLTAGE;

  //getting new state
  if(systemOn){
    if(manualMode){
      currentState = MANUAL_STATE;
    }
    else{
      currentState = AUTO_STATE;
    }
  }
  else{
    currentState = OFF_STATE;
  }

  //setting power depending on state
  if(currentState == AUTO_STATE){
      int voltage = analogRead(TEMP_PIN) * (3300.0 / 1024.0);

      // now print out the temperature
      float temperatureF = (voltage - 500) / 10 ;
      float temperatureC = (temperatureF - 32) * 5 / 9;

      // Reading potentiometer value
      potValue = analogRead(POT_PIN);


      //do temperature voltage calculations here
      //read desired value first, here uses fakeTemp as a temperature caculated above.
      if (fakeTemp<=desiredTemp){
        digitalWrite(DIGITAL_FAN_PIN,LOW);
      }
      else if (desiredTemp<fakeTemp<=(desiredTemp+5)){
        digitalWrite(DIGITAL_FAN_PIN,HIGH);
        currentFanPower = LOW_VOLTAGE;
        lcd.setCursor(15,1);
        lcd.print("L");

      }
      else if((desiredTemp+5)<fakeTemp<=(desiredTemp+10)){
        digitalWrite(DIGITAL_FAN_PIN,HIGH);
        currentFanPower = MEDIUM_VOLTAGE;
        lcd.setCursor(15,1);
        lcd.print("M");
      }
      else if(fakeTemp>desiredTemp+10){
        digitalWrite(DIGITAL_FAN_PIN,HIGH);
        currentFanPower = HIGH_VOLTAGE;
        lcd.setCursor(15,1);
        lcd.print("H");
      }

  }
  else if(currentState == MANUAL_STATE){
      currentFanPower = manualFanPower;
  }

  //changing lcd writing if needed
  if(currentState != previousState || currentFanPower != previousFanPower){
    //lcd.clear();
    if(currentState == OFF_STATE){
      //lcd.print("System Off");
      //line0[LCD_LEN] = ' ';
      strncpy(line0," ",LCD_LEN);
      strncpy(line0,"Off",LCD_LEN);
      updateDisplay();
      //line0[15] = 'F';
    }
    else if(currentState == AUTO_STATE){
      //lcd.print("System On(AUTO)");
      //line0[LCD_LEN] = ' ';
      strncpy(line0," ",LCD_LEN);
      strncpy(line0,"On(AUTO)",LCD_LEN);
      updateDisplay();
      //line0[15] = 'A';
    }
    else if(currentState == MANUAL_STATE){
      //lcd.print("System On(MANUAL)");
      //line0[LCD_LEN] = ' ';
      strncpy(line0," ",LCD_LEN);
      strncpy(line0,"On(MANU)",LCD_LEN);
      updateDisplay();
      //line0[15] = 'M';
    }
    //lcd.setCursor(0, 1);
    //lcd.print(currentFanPower);
  }
  //this should work but for some reason the analoge writing isn't working, swap this for the digital write to see it working
  //dac_output_voltage(ANALOG_FAN_PIN, currentFanPower);
  digitalWrite(DIGITAL_FAN_PIN, currentFanPower == OFF_VOLTAGE ? LOW : HIGH);


  previousState = currentState;
  previousFanPower = currentFanPower;
}

//clear screen whenever get update
void updateDisplay() {//No.3
   //lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(line0);
   //lcd.print(line1); // failed to assine a int value to char[]
   lcd.setCursor(9,0);
   lcd.print(tempC);
   //SYS STATE
   lcd.setCursor(0,1);
   if(currentState == OFF_STATE){
     lcd.print("OFF");
   }
   else if(currentState == AUTO_STATE){
     lcd.print("AUTO");
   }
   else if(currentState == MANUAL_STATE){
     lcd.print("MANUAL");
   }
   //FAKE TEMP
   lcd.setCursor(9,1);
   lcd.print(fakeTemp);
   //FAN SPEED
   lcd.setCursor(15,1);
   if(currentFanPower == LOW_VOLTAGE){
     lcd.print("L");
   }
   else if(currentFanPower == MEDIUM_VOLTAGE){
     lcd.print("M");
   }
   else if(currentFanPower == HIGH_VOLTAGE){
     lcd.print("H");
   }
   //lcd.print("H");
   //lcd.print(tempF);
   //print fake temp

}

void sendSensor(){ // No.1
  lcd.clear();
  tempVal = analogRead(TEMP_PIN);
  volts = tempVal * 3.3/1024;// arduino uses 3.3v
  tempC = (volts - 0.5) * 100 ; //C with 0.5v offset
  tempF = (tempC * 9.0 / 5.0) + 32.0;//Fahrenheit
  if(isnan(tempVal)){
    Serial.println("Failed to read from sensor");
    lcd.print("Failed to read sensor data\n");
    return;
  }
  line0[LCD_LEN] = ' ';
  strncpy(line0,"TmpNowC",LCD_LEN);

}

void setup() {
  Serial.begin(9600);
  // Setup a function to be called every second
  fanUpdateTimer.setInterval(1000L,sendSensor);
  fanUpdateTimer.setInterval(1000L,fanUpdateEvent);
  fanUpdateTimer.setInterval(1000L,updateDisplay);

  lcd.begin(16,2);

  systemOn = false;
  manualMode = false;

  currentState = OFF_STATE;
  previousState = -1;

  desiredTemp = 22;

  manualFanPower = OFF_VOLTAGE;
  currentFanPower = OFF_VOLTAGE;
  previousFanPower = OFF_VOLTAGE;

  //reenable this to fix the analogue pin
  //dac_output_enable(ANALOG_FAN_PIN);
  //dac_output_voltage(ANALOG_FAN_PIN, currentFanPower);
  pinMode(DIGITAL_FAN_PIN, OUTPUT);
  digitalWrite(DIGITAL_FAN_PIN, currentFanPower == OFF_VOLTAGE ? LOW : HIGH);

  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASS);
}

void loop() {
  Blynk.run();
  fanUpdateTimer.run();


}

BLYNK_CONNECTED() {
  Serial.println("Connected to app!");
  Blynk.virtualWrite(V1, systemOn ? 1 : 0);
  Blynk.virtualWrite(V2, manualMode ? 1 : 0);
  Blynk.virtualWrite(V3, desiredTemp);

  int index = 0;
  if(manualMode){
    if(currentFanPower == LOW_VOLTAGE){
      index = 1;
    }
    else if(currentFanPower == MEDIUM_VOLTAGE){
      index = 2;
    }
    else if(currentFanPower == HIGH_VOLTAGE){
      index = 3;
    }

  }
  Blynk.virtualWrite(V4, index);
}

BLYNK_WRITE(V1) {
  if(param.asInt() == 1){
    systemOn = true;
  }
  else{
    systemOn = false;
    manualMode = false;
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V4, 0);
  }
}

BLYNK_WRITE(V2) {
  if(param.asInt() == 1){
    if(systemOn){
      manualMode = true;
      Blynk.virtualWrite(V4, 1);
      manualFanPower = LOW_VOLTAGE;
    }
    else{
      Blynk.virtualWrite(V2, 0);
    }
  }
  else{
     manualMode = false;
     Blynk.virtualWrite(V4, 0);
  }
}

BLYNK_WRITE(V5) {
  desiredTemp = param.asInt();
}

//read fake temp
BLYNK_WRITE(V6) {
  fakeTemp = param.asInt();
}

BLYNK_WRITE(V4){
  if(manualMode){
    switch(param.asInt()){
      case 1:{
        manualFanPower = LOW_VOLTAGE;
        break;
      }
      case 2:{
        manualFanPower = MEDIUM_VOLTAGE;
        break;
      }
      case 3:{
        manualFanPower = HIGH_VOLTAGE;
        break;
      }
    }
  }
  else{
    manualFanPower = OFF_VOLTAGE;
    Blynk.virtualWrite(V4, 0);
  }
}
