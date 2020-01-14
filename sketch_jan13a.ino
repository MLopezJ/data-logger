#include <Time.h>
#include <TimeLib.h>
#include <SPI.h>
#include <SD.h>

// Set the pins used
#define cardSelect 4
int MQ04 = 0;

File logfile;
File dateFile;

char filename[15];
String day_var = "" ;
String month_var = "" ;
String year_var = "" ;
String hour_var = "";
String minute_var = "";
String second_var = "";


// blink out an error code
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}

void setTimeFromTxt(){
 
  dateFile = SD.open("DATE.TXT",FILE_READ);
 
  if(dateFile){
    char dateTxt[19];
    memset(dateTxt,0,sizeof(dateTxt));
    char Char[2];
    memset(Char,0,sizeof(Char));
   
    while (dateFile.available()){
      Char[0] = dateFile.read();
      strcat(dateTxt,Char);
      delay(100);
    }
    Serial.println(dateTxt);
    dateFile.close();
    
    day_var = String(dateTxt[0]) + String(dateTxt[1]);
    month_var = String(dateTxt[3]) + String(dateTxt[4]);
    year_var = String(dateTxt[6]) + String(dateTxt[7]) + String(dateTxt[8]) + String(dateTxt[9]);
    hour_var = String(dateTxt[11]) + String(dateTxt[12]);
    minute_var = String(dateTxt[14]) + String(dateTxt[15]);
    second_var = String(dateTxt[17]) + String(dateTxt[18]);

    Serial.println(day_var);
    Serial.println(month_var);
    Serial.println(year_var);
    Serial.println(hour_var);
    Serial.println(minute_var);
    Serial.println(second_var);
    
    setTime(hour_var.toInt(),minute_var.toInt(),second_var.toInt(),day_var.toInt(),month_var.toInt(),year_var.toInt());
  }
}

String twoDigits(String var){
  
  int value = var.toInt();
  String stringValue;
  
  if (value < 10){
    stringValue = '0' + String(var);
  }
  else{
    stringValue = String(var);
  }
  
  return stringValue;
}

void createAnalogFile(){
  
  strcpy(filename, "/ANALOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create ");
    Serial.println(filename);
    error(3);
  }
  else{
    Serial.print("File created: ");
    Serial.println(filename);
    pinMode(13, OUTPUT);
    pinMode(8, OUTPUT);
    Serial.println("Ready!");
    logfile.close();
  }
  
}

// This line is not needed if you have Adafruit SAMD board package 1.6.2+
//   #define Serial SerialUSB

void setup() {
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(9600);
 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
    error(2);
  }

  delay(3000);

  Serial.println("initialization done.");
 
  setTimeFromTxt();
 
  pinMode(13, OUTPUT);

  createAnalogFile();
  
}

String getDate(){
  time_t t = now();//Declaramos la variable time_t t
  delay(1000);//Esperamos 1 segundo
  day_var = twoDigits(String(day(t)));
  month_var = twoDigits(String(month(t)));
  year_var = twoDigits(String(year(t)));
  hour_var = twoDigits(String(hour(t)));
  minute_var = twoDigits(String(minute(t)));
  second_var = twoDigits(String(second(t)));
  String returnDate = day_var + "/" + month_var + "/" + year_var + " " + hour_var + ":" + minute_var + ":" + second_var;
  return returnDate;
}

uint8_t i=0;
void loop() {
  

  String currentDate = getDate();
  Serial.println(currentDate);
  Serial.println(filename);

  /*
  digitalWrite(8, HIGH);
  logfile.println(analogRead(0));
  logfile.flush();
  Serial.print("MQ04 = ");
  Serial.println(analogRead(0));
  digitalWrite(8, LOW);
 
  delay(100);
  */

  //LOGGER
  logfile = SD.open(filename, FILE_WRITE);
  if (logfile) {
    logfile.print(analogRead(0));
    logfile.print(",");
    logfile.println(currentDate);
    logfile.flush();
    logfile.close();
    
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening DATE.TXT");
  }

  delay(5000);

  //DATE
  SD.remove("DATE.TXT");
  dateFile = SD.open("DATE.TXT", FILE_WRITE);
  if (dateFile) {
    dateFile.println(currentDate);
    dateFile.flush();
    dateFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening DATE.TXT");
  }

  delay(5000);

}
