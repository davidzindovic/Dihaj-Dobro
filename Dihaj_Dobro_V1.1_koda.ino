/*
   Dihaj Dobro 1.1 software by David Zindović

   Desired parameter values:
   TEMPERATURE: 26°c+
   HUMIDITY:  50%<
   CO2 LEVEL: 800ppm-1000ppm> ~> PREVERI TA POGOJ
*/
//TO DO: poštudiri google sheets plot/post
/*#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <Adafruit_NeoPixel.h>
#include "FS.h"

#define BUZZER 13
//#define CS 14
//D1, D2 BME680
#define LEDPIN 12
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

//TO DO: PREVERI POGOJE ZA BARVO IN TON OB PRVI IZVEDBI; nastavi meje parametrov

//preveri vrednosti in ustrezno postavi limite
#define CO2_GOOD  250
#define HUMID_GOOD 50
#define TEMP_GOOD 26

#define CO2_LIMIT 100
#define HUMID_LIMIT 15
#define TEMP_LIMIT 5 //premisli zaradi okna v zimskih časih

#define EXPECTED_CO2_RANGE 200 
#define EXPECTED_HUMID_RANGE 50
#define EXPECTED_TEMP_RANGE 15

#define MEASURMENT_REPETITION 10

#define DELAYVAL 500
#define MINI_DELAY 10//ZA VZPOSTAVITEV NORMALNEGA DELOVANJA uC
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

int current_temperature, current_humidity, current_co2;int tone_counter=0;

void setup(){
  // Serial port for debugging purposes
  delay(10);
  Serial.begin(115200);
  Serial.println();
  pinMode(BUZZER, OUTPUT);
  pixels.begin();
  bme.begin();
 
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms


  measurment_gradient(TEMP_GOOD,HUMID_GOOD,CO2_GOOD);//ob bootu se pokaže zelena, potem fadea v current
  delay(500);
  }

void measurment_gradient(int temperature, int co2, int humidity)
{//IDEA--> ZAČNI Z ZELENO IN MODIFICIRAJ BARVO Z KOEFICIENTI
  delay(5);pixels.clear();delay(5);
  int temperature_color, co2_color, humidity_color, fade_steps, former_temp=0, former_co2=0, former_humid=0;
  Serial.println("lucke");
  if(TEMP_GOOD-TEMP_LIMIT<=temperature&&temperature<=TEMP_GOOD+TEMP_LIMIT) {temperature_color=0;}//ok ce je znotraj obmocja
  else if(TEMP_GOOD-TEMP_LIMIT>temperature){temperature_color=(abs(TEMP_GOOD-temperature))*(255/EXPECTED_TEMP_RANGE);}
  else if(TEMP_GOOD+TEMP_LIMIT<temperature){temperature_color=(abs(temperature-TEMP_GOOD))*(255/EXPECTED_TEMP_RANGE);}

  if(CO2_GOOD-CO2_LIMIT<=co2&&co2<=CO2_GOOD+CO2_LIMIT) {co2_color=255;}//ce prekoraci=problem
  else if(CO2_GOOD-CO2_LIMIT>co2){co2_color=255-(abs(co2-CO2_GOOD))*(255/EXPECTED_CO2_RANGE);}//ce doseze co2 max limit ne bo več zelenega
  else if(CO2_GOOD+CO2_LIMIT<co2){co2_color=255;}
  
  if(HUMID_GOOD-HUMID_LIMIT<=humidity&&humidity<=HUMID_GOOD+HUMID_LIMIT){humidity_color=0;}
  else if(HUMID_GOOD-HUMID_LIMIT>humidity){humidity_color=(HUMID_GOOD-humidity)*(255/EXPECTED_HUMID_RANGE);}
  else if(HUMID_GOOD+HUMID_LIMIT<humidity){humidity_color=(humidity-HUMID_GOOD)*(255/EXPECTED_HUMID_RANGE);}
 /* if(abs(TEMP_GOOD-temperature)-TEMP_LIMIT=>abs(HUMID_GOOD-humidity)-HUMID_LIMIT&&abs(TEMP_GOOD-temperature)-TEMP_LIMIT=>abs(CO2_GOOD-co2)-CO2_LIMIT)
    {fade_steps=abs(TEMP_GOOD-temperature)-TEMP_LIMIT;}
  else if(abs(CO2_GOOD-co2)-CO2_LIMIT=>abs(HUMID_GOOD-humidity)-HUMID_LIMIT&&abs(CO2_GOOD-co2)-CO2_LIMIT=>abs(TEMP_GOOD-temperature)-TEMP_LIMIT)
    {fade_steps=abs(CO2_GOOD-co2)-CO2_LIMIT;}
  else if(abs(HUMID_GOOD-humidity)-HUMID_LIMIT=>abs(TEMP_GOOD-temperature)-TEMP_LIMIT&&abs(HUMID_GOOD-humidity)-HUMID_LIMIT=>abs(CO2_GOOD-co2)-CO2_LIMIT)
    {fade_steps=abs(HUMID_GOOD-humidity)-HUMID_LIMIT;)*/
//poglej ce bo kul else if in ta former color method
  /*if(abs(former_temp-temperature_color)>=abs(former_humid-humidity_color)&&abs(former_temp-temperature_color)>=abs(former_co2-co2_color))
    {fade_steps=abs(former_temp-temperature_color);}
  else if(abs(former_humid-humidity_color)>=abs(former_temp-temperature_color)&&abs(former_humid-humidity_color)>=abs(former_co2-co2_color))
    {fade_steps=abs(former_humid-humidity_color);}
  else if(abs(former_co2-co2_color)>=abs(former_temp-temperature_color)&&abs(former_co2-co2_color)>=abs(former_humid-humidity_color))
    {fade_steps=abs(former_co2-co2_color);}
    Serial.println();
    Serial.print("Fade steps lucke: "); Serial.println(fade_steps);*/
  /*  fade_steps=10;
  for(int i=0;i<fade_steps;i++)
    {
      pixels.clear();
      pixels.setPixelColor(0,pixels.Color(former_temp+round(abs(temperature_color-former_temp)/fade_steps),former_co2+round(abs(co2_color-former_co2)/fade_steps),former_humid+round(abs(humidity_color-former_humid)/fade_steps)));
      delay(10);pixels.show();
    }*/
    if(temperature_color>255){temperature_color=255-round(temperature_color/255);}
    if(humidity_color>255){humidity_color=255-round(humidity_color/255);}
    if(co2_color>255){co2_color=255-round(co2_color/255);}
  
Serial.print("barva temperature: ");Serial.print(temperature_color);
Serial.print(" barva vlage: ");Serial.print(humidity_color);
Serial.print(" barva co2 :");Serial.println(co2_color);
    
    pixels.setPixelColor(0,pixels.Color(temperature_color,co2_color,humidity_color));
    delay(5);pixels.show();delay(10);//zakaj ne sveti pri00255
  former_temp=temperature_color; former_humid=humidity_color; former_co2=co2_color;
/*Serial.print("\n Prejšnje barve: temp: "); Serial.print(former_temp);
Serial.print(" humid: "); Serial.print(former_humid);
Serial.print(" co2: "); Serial.print(former_co2);*/
}


void measurment_tone(int tone_temperature, int tone_co2, int tone_humidity)
{ int good_frequency=1000; int good_fade=1; int good_duration=1000;
  int frequency, fade, duration;
  if(tone_counter>(5000/(tone_temperature+tone_co2+tone_humidity))){tone_counter=0;//double check pogoj za pisk - imenovalec)
  if(tone_temperature<=1){frequency=good_frequency;}else{int frequency=tone_temperature*1000;}
  if(tone_co2<=1){fade=good_fade;}else{int fade=tone_co2;}
  if(tone_humidity<=1){duration=good_duration;}else{int duration=tone_humidity*1500;}
  /*
  if(tone_temperature>1){int frequency=tone_temperature;}
  if(tone_co2>1){int fade=tone_co2;}
  if(tone_humidity>1){int duration=tone_humidity;}*/

  Serial.println("Pjevam \n");
  if(fade>1){
  for(int t=1;t<(duration/10);t++)
  {//poglej zakaj ne piska
    Serial.println(fade);
    tone(BUZZER,frequency-t*(frequency/(duration/10)),(duration/10)/10);
    delay((duration/10)/10+1);
  }}
//  Serial.print("frequency: ");Serial.print(frequency);Serial.print("  duration: ");Serial.println(duration);
  else{tone(BUZZER,frequency,duration);}
Serial.print("frequency: ");Serial.print(frequency);Serial.print("  duration: ");Serial.println(duration);
  }else{tone_counter+=1;}
}


void measurment_analysis()
{int temp_sum=0; int humid_sum=0; int co2_sum=0;
for(int q=0;q<MEASURMENT_REPETITION;q++)
{
  temp_sum+=bme.temperature-5;
  humid_sum+=bme.humidity;
  co2_sum+=(bme.gas_resistance)/1000;
  delay(10);
}
 /* float current_temperature=bme.temperature-5;
  float current_humidity=bme.humidity;
  float current_co2=(bme.gas_resistance)/1000;
  Serial.println("meritve:\n");
  Serial.print("Temperatura: ");Serial.print(current_temperature);
  Serial.print("  Vlaga: ");Serial.print(current_humidity);
  Serial.print("  CO2: "); Serial.println(current_co2);*/
 float current_temperature=temp_sum/MEASURMENT_REPETITION;Serial.print("Temperatura: ");Serial.print(current_temperature);
 float current_humidity=humid_sum/MEASURMENT_REPETITION;Serial.print(" Vlaga: ");Serial.print(current_humidity);
 float current_co2=co2_sum/MEASURMENT_REPETITION;Serial.print(" CO2: ");Serial.println(current_co2);
Serial.println();  
  float temperature_warning=round(abs(current_temperature-TEMP_GOOD)/TEMP_LIMIT);
  float co2_warning=round(abs(current_co2-CO2_GOOD)/CO2_LIMIT);
  float humidity_warning=round(abs(current_humidity-HUMID_GOOD)/HUMID_LIMIT);
Serial.print("temperature_warning: ");Serial.println(temperature_warning);
Serial.print("CO2 warning: ");Serial.println(co2_warning);
Serial.print("humidity warning: ");Serial.println(humidity_warning);
  measurment_gradient(current_temperature, current_co2, current_humidity);
  measurment_tone(temperature_warning, co2_warning, humidity_warning);
  Serial.println(tone_counter);
  Serial.println("konec analize");
}

void error()
{
  for(int a=1;a<5;a++)
  {
    pixels.clear();
    pixels.setPixelColor(0,(255,0,0));
    if(a%2!=0){pixels.show(); delay(1000);}
    else{delay(1000);}
  }
}

void loop(){
 if (! bme.performReading()) {
  error();}

    measurment_analysis();



}
