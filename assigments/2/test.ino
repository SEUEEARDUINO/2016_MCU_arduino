#include <Wire.h>
#include <DateTime.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>


int addr = 0,address = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int year = 2016;
byte month = 5, day = 6, hour = 23, minute = 41, second = 23;
int year_now;
byte month_now, day_now, hour_now, minute_now, second_now;
int k = 0;

int length;
int tonepin=6;   //得用6号接口
#define NTD0 -1
#define NTD1 294
#define NTD2 330
#define NTD3 350
#define NTD4 393
#define NTD5 441
#define NTD6 495
#define NTD7 556
 
int tune[]= 
{
  NTD3,NTD3,NTD4,NTD5,
  NTD5,NTD4,NTD3,NTD2,
  NTD1,NTD1,NTD2,NTD3,
  NTD3,NTD2,NTD2,
  NTD3,NTD3,NTD4,NTD5,
  NTD5,NTD4,NTD3,NTD2,
  NTD1,NTD1,NTD2,NTD3,
  NTD2,NTD1,NTD1,
  NTD2,NTD2,NTD3,NTD1,
  NTD2,NTD3,NTD4,NTD3,NTD1,
  NTD2,NTD3,NTD4,NTD3,NTD2,
  NTD1,NTD2,NTD5,NTD0,
  NTD3,NTD3,NTD4,NTD5,
  NTD5,NTD4,NTD3,NTD4,NTD2,
  NTD1,NTD1,NTD2,NTD3,
  NTD2,NTD1,NTD1
  };
  float durt[]= 
  { 1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1+0.5,0.5,1+1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,1,
  1+0.5,0.5,1+1,
  1,1,1,1,
  1,0.5,0.5,1,1,
  1,0.5,0.5,1,1,
  1,1,1,1,
  1,1,1,1,
  1,1,1,0.5,0.5,
  1,1,1,1,
  1+0.5,0.5,1+1,
  };

String data = "";
int data_int[20];
int mark = 0;

const int TrigPin = 8; 
const int EchoPin = 9;
float cm; 

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(tonepin,OUTPUT);
  pinMode(TrigPin, OUTPUT); 
  pinMode(EchoPin, INPUT);
  //读取断电之前的时间
  year = EEPROM.read(address);
  address = address + 1;
  year = year*100 + EEPROM.read(address);
  address = address + 1;
  month = EEPROM.read(address);
  address = address + 1;
  day = EEPROM.read(address);
  address = address + 1;
  hour = EEPROM.read(address);
  address = address + 1;
  minute = EEPROM.read(address);
  address = address + 1;
  second = EEPROM.read(address);
  address = address + 1;
}

void loop() {
  //显示温度值
  ultrasonic();
  if(millis()%100 == 0) printtemp();
  //读取串口数据，保存为字符串
  data = "";
  while(Serial.available())
  {
    data += char(Serial.read());
    delay(5);
    //标志读入了字符串
    mark = 1;
    Serial.println(data);
  }
  if(mark == 1) deal_string(data);  
  printtime();
}



//处理串口接收到的字符串
void deal_string(String data)
{
      //串口输出温度
    if(data == "temp")Serial.println(printtemp());
    else
    {
      for(int i = 0;i<data.length(); i++)
      {
        //将字符串转换成整数型
        data_int[i] = data[i] - '0';
      }
      //保存收到的时间
      year = 1000*data_int[0]  + 100*data_int[1]  + 10*data_int[2]  + data_int[3] ;
      month = 10*data_int[5] + data_int[6] ;
      day =  10*data_int[8]  + data_int[9] ;
      hour =  10*data_int[11]  + data_int[12] ;
      minute =  10*data_int[14]  + data_int[15] ;
      second = 10*data_int[17] + data_int[18] ;
      //记录下此时开机经过的秒数
      k = millis()/1000;
    }
    data = "";
    mark = 0;
}
//在lcd上显示时间
void printtime()
{
  //获取当前时间到上一次重置时间的秒数
  int t = millis()/1000 - k ;
  //计算当前时间
  second_now = (second+t)%60;
  minute_now = ((second+t)/60+minute)%60;
  hour_now = (((second+t)/60+minute)/60+hour)%24;
  day_now = ((((second+t)/60+minute)/60+hour)/24+day)%30;
  month_now = (((((second+t)/60+minute)/60+hour)/24+day)/30+month)%12;
  year_now = (((((second+t)/60+minute)/60+hour)/24+day)/30+month)/12+year;
  //在lcd上显示时间
  lcd.setCursor(0, 0);
  lcd.print(year_now);
  lcd.setCursor(4, 0);  
  lcd.print('/');
  lcd.setCursor(5, 0);  
  if(month_now<10)
  {
    lcd.print(0);
    lcd.setCursor(6,0);
  }
  lcd.print(month_now);
  lcd.setCursor(7, 0);  
  lcd.print('/');
  lcd.setCursor(8, 0);  
  if(day_now<10)
  {
    lcd.print(0);
    lcd.setCursor(9,0);
  }
  lcd.print(day_now);    
  lcd.setCursor(0, 1);
  if(hour_now<10)
  {
    lcd.print(0);
    lcd.setCursor(1,1);
  } 
  lcd.print(hour_now);
  lcd.setCursor(2, 1);  
  lcd.print(':');
  lcd.setCursor(3, 1); 
  //如果小于10，补上0 
  if(minute_now<10)
  {
    lcd.print(0);
    lcd.setCursor(4,1);
  }
  lcd.print(minute_now);
  lcd.setCursor(5, 1);  
  lcd.print(':');
  lcd.setCursor(6, 1);  
  if(second_now<10)
  {
    lcd.print(0);
    lcd.setCursor(7,1);
  }
  lcd.print(second_now);
  //将当前时间保存到EEPROM中
  EEPROM.write(addr, year_now/100); 
  addr = addr + 1;
  EEPROM.write(addr, year_now%100); 
  addr = addr + 1;
  EEPROM.write(addr, month_now); 
  addr = addr + 1;
  EEPROM.write(addr, day_now); 
  addr = addr + 1;
  EEPROM.write(addr, hour_now); 
  addr = addr + 1;
  EEPROM.write(addr, minute_now); 
  addr = addr + 1;
  EEPROM.write(addr, second_now); 
  addr = 0;
}
//读取并返回温度
float printtemp(){
    int n = analogRead(A5); //读取A0口的电压值    
    float vol = n * (5.0 / 1023.0*100);    
    lcd.setCursor(9, 1);
    lcd.print(vol);
    if(vol > 40)playmusic();
    return vol;
}
//演奏音乐
void playmusic()
{
  length=sizeof(tune)/sizeof(tune[0]);   //计算长度
  for(int x=0;x<length;x++)    
  {
    tone(tonepin,tune[x]);
    delay(500*durt[x]);   //这里用来根据节拍调节延时
    noTone(tonepin);
    printtime();
   }
}
//超声波测距
void ultrasonic()
{
  digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin 
 delayMicroseconds(2); 
 digitalWrite(TrigPin, HIGH); 
 delayMicroseconds(10); 
 digitalWrite(TrigPin, LOW); 

 cm = pulseIn(EchoPin, HIGH) / 58.0; //将回波时间换算成cm 
 cm = (int(cm * 100.0)) / 100.0; //保留两位小数 
 //小于5cm发出警报
 if(cm<5)playmusic();
}

