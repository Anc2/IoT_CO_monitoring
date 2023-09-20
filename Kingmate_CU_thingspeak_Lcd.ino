#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Thingspeak  
String statusChWriteKey = "JM4U311Z22G153F8";  // Status Channel id: 385184

#include <SoftwareSerial.h>
SoftwareSerial EspSerial(4, 5); // Rx,  Tx
#define HARDWARE_RESET 6
LiquidCrystal_I2C lcd(0x27,16,2);


// constants won't change. Used here to set a pin number:
const int redLED =  7;// the number of the LED pin
const int blueLED =  8;// the number of the LED pin
 int buzzer = 9;

 bool iotBuzzer = 0;
 bool iotRedLED = 0;
 bool iotBlueLED = 0;
 
// Variables will change:
int ledState = LOW;             // ledState used to set the LED
int buzzerState = LOW;
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 500;           // interval at which to blink (milliseconds)

int ledState1 = LOW; 
unsigned long previousMillis1 = 0;  


int sensorValue, sensorValue2;

// Variables to be used with timers
long writeTimingSeconds = 17; // ==> Define Sample time in seconds to send data
long startWriteTiming = 0;
long elapsedWriteTime = 0;

int spare = 0;
boolean error;




void setup(){
  Serial.begin(9600);
  
  pinMode(HARDWARE_RESET,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  
  digitalWrite(HARDWARE_RESET, HIGH);
   EspSerial.begin(9600);
  EspHardwareReset(); 
  startWriteTiming = millis(); // starting the "program clock"

lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("    WELCOME    ");
   lcd.setCursor(0,1);
  lcd.print("MISTER KINGMATE ");
  delay(2000);
   lcd.setCursor(0,0);
  lcd.print(" IoT BASED CO &");
   lcd.setCursor(0,1);
  lcd.print("  CO2 MONITORING ");
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print(" CO2 MONITORING");
   lcd.setCursor(0,1);
  lcd.print("     PROJECT      ");
  delay(2000);
  lcd.clear();
}

void redled(){
  unsigned long currentMillis1 = millis();

  if (currentMillis1 - previousMillis1 >= interval) {
    // save the last time you blinked the LED
    previousMillis1 = currentMillis1;

    // if the LED is off turn it on and vice-versa:
    if (ledState1 == LOW) {
      ledState1 = HIGH;
    } else {
      ledState1 = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(redLED, ledState1);
  }
}


void blueled(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(blueLED, ledState);
  }
}


void buzzerOn(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (buzzerState == LOW) {
      buzzerState = HIGH;
    } else {
      buzzerState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(buzzer, buzzerState);
  }
}



void loop(){
  start: //label 
  error=0;
  
  elapsedWriteTime = millis()-startWriteTiming; 
  
  if (elapsedWriteTime > (writeTimingSeconds*1000)) {

   readSensors();

    
    
    writeThingSpeak();
    startWriteTiming = millis();   
  }
  
  if (error==1) //Resend if transmission is not completed 
  {       
    Serial.println(" <<<< ERROR >>>>");
    delay (2000);  
    goto start; //go to label "start"
  }
}

/********* Read Sensors value *************/
void readSensors(void){
  sensorValue = analogRead(A0);
   
   sensorValue2 = analogRead(A1);

  
  

 if ((sensorValue2 >= 170) || (sensorValue >=170)){
  iotBuzzer = 1;
 iotRedLED = 1;
 iotBlueLED = 0;
 
 digitalWrite(buzzer, HIGH);
      redled();      
     // Serial.println("Environment too harrmful");
   lcd.setCursor(0,0);
  lcd.print("VICINITY HARMFUL");
   lcd.setCursor(0,1);
  lcd.print("PLS STAY OFF!!!");
  
       
  }

if ((sensorValue2 <= 160) || (sensorValue <= 160) ){
    iotBuzzer = 0;
 iotRedLED = 0;
  iotBlueLED = 1;
 
  digitalWrite(buzzer,LOW);
   blueled();     
     // Serial.println("ALL GOOD! ");

 lcd.setCursor(0,0);
  lcd.print("CO LEVEL: ");
  lcd.print(sensorValue);
  lcd.print("ppm   ");
   lcd.setCursor(0,1);
  lcd.print("CO2 LEVEL: ");
  lcd.print(sensorValue2);
  lcd.print("ppm");
  }

 Serial.print("sensorValue - ");
  Serial.println(sensorValue);  
    Serial.print("sensorValue2 - ");
     Serial.println(sensorValue2);
      Serial.print(" ");
    Serial.print(iotBuzzer);
    Serial.print(" ");
    Serial.print(iotRedLED);
     Serial.print(" ");
    Serial.println(iotBlueLED);
    //delay(wait); 

}

/********* Conexao com TCP com Thingspeak *******/
void writeThingSpeak(void){

  startThingSpeakCmd();

  // preparacao da string GET
  String getStr = "GET /update?api_key=";
  getStr += statusChWriteKey;
  getStr +="&field1=";
  getStr += String(sensorValue);
  getStr +="&field2=";
  getStr += String(sensorValue2);
  getStr +="&field3=";
  getStr += String(iotBuzzer);
  getStr +="&field4=";
  getStr += String(iotRedLED);
  getStr +="&field5=";
  getStr += String(iotBlueLED);
  getStr += "\r\n\r\n";

  sendThingSpeakGetCmd(getStr); 
}

/********* Reset ESP *************/
void EspHardwareReset(void){
  Serial.println("Reseting......."); 
  digitalWrite(HARDWARE_RESET, LOW); 
  delay(500);
  digitalWrite(HARDWARE_RESET, HIGH);
  delay(8000);//Tempo necessário para começar a ler 
  Serial.println("RESET"); 
}

/********* Start communication with ThingSpeak*************/
void startThingSpeakCmd(void)
{
  EspSerial.flush();//limpa o buffer antes de começar a gravar
  
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // Endereco IP de api.thingspeak.com
  cmd += "\",80";
  EspSerial.println(cmd);
  Serial.print("enviado ==> Start cmd: ");
  Serial.println(cmd);

  if(EspSerial.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}

/********* send a GET cmd to ThingSpeak *************/
String sendThingSpeakGetCmd(String getStr)
{
  String cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  EspSerial.println(cmd);
  Serial.print("enviado ==> lenght cmd: ");
  Serial.println(cmd);

  if(EspSerial.find((char *)">"))
  {
    EspSerial.print(getStr);
    Serial.print("enviado ==> getStr: ");
    Serial.println(getStr);
    delay(500);//tempo para processar o GET, sem este delay apresenta busy no próximo comando

    String messageBody = "";
    while (EspSerial.available()) 
    {
      String line = EspSerial.readStringUntil('\n');
      if (line.length() == 1) 
      { //actual content starts after empty line (that has length 1)
        messageBody = EspSerial.readStringUntil('\n');
      }
    }
    Serial.print("MessageBody received: ");
    Serial.println(messageBody);
    return messageBody;
  }
  else
  {
    EspSerial.println("AT+CIPCLOSE");     // alert user
    Serial.println("ESP8266 CIPSEND ERROR: RESENDING"); //Resend...
    spare = spare + 1;
    error=1;
    return "error";
  } 
}
