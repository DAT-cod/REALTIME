#include "DHT.h"

#define led1  A0
#define led2  A1
#define led3  A2
#define led4  A3
#define DHTPIN 2
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
#define led_on(pin) digitalWrite(pin, LOW)
#define led_off(pin) digitalWrite(pin, HIGH)
#define _read(pin) digitalRead(pin)
#define output_low(pin) digitalWrite(pin, LOW)
#define output_high(pin) digitalWrite(pin, HIGH)

#define CS  3
#define MOSI  6
#define MISO  5
#define SCK 4

#define REG_NHIET_DO 0x01
#define REG_DO_AM 0x02
#define REG_OUTPUT 0x03
#define CMD_READ 0x15
#define CMD_WRITE 0x20


unsigned char nhiet_do;
unsigned char do_am;

void setup() 
{
  pinMode(MISO, OUTPUT);
  pinMode(CS, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  led_off(led1);
  led_off(led2);
  led_off(led3);
  led_off(led4);
  
  Serial.begin(9600);
  Serial.println("run application");
  dht.begin();
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), spi_slave_read, FALLING);
}//setup

void loop() 
{
  do_am = dht.readHumidity();
  nhiet_do = dht.readTemperature();

  Serial.print(F("Humidity: "));
  Serial.print(do_am);
  Serial.print(F("%  Temperature: "));
  Serial.print(nhiet_do);
  Serial.println(F("°C "));


  delay(500);

}//void loop



unsigned char spi_slave_read()
{
  unsigned char read_value;
  unsigned char write_value;
  unsigned char _bit=0;
  unsigned char count=0;
  if(!_read(CS))
  {
    //read data
    for(count=0;count<8;count++)
    {
      _bit=0;
      while(_read(SCK)) _bit=_read(MOSI);
      read_value|=_bit;
      read_value<<=1;
      while(!_read(SCK));
    }

    //check register
    switch(read_value)
    {
      case REG_NHIET_DO:  write_value=nhiet_do; break;
      case REG_DO_AM: write_value=do_am; break;
      //case REG_OUTPUT:  write_value= break;
      default:;
    }
    
    //write data
    for(count=0;count<8;count++)
    {
      _bit=write_value&(0x80>>count);
      while(_read(SCK)) _bit?output_high(MISO):output_low(MISO);
      while(!_read(SCK));
    }
    while(!_read(CS));
  }
  return read_value;
}
