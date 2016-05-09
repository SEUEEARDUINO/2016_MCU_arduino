#include "pitches.h"
#include<LiquidCrystal.h>
#include<EEPROM.h>

int potPin = 0; //定义模拟接口0 连接LM35 温度传感器
int buzzer=8;//设置控制蜂鸣器的数字IO脚

// 定义超声波模块的引脚
const int TrigPin = 7;
const int EchoPin = 6;

// 定义彩灯模块的引脚
int redpin = 9;
int bluepin = 10;
int greenpin = 13;
char wby;

// 测距的距离变量
float distance=100;

// 播放乐曲的长度
int lengths;

int melody[] = {
NOTE_DH1,NOTE_D6,NOTE_D5,NOTE_D6,NOTE_D0,  
NOTE_DH1,NOTE_D6,NOTE_D5,NOTE_DH1,NOTE_D6,NOTE_D0,NOTE_D6, 
NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D6,NOTE_D0,NOTE_D6, 
NOTE_DH1,NOTE_D6,NOTE_D5,NOTE_DH1,NOTE_D6,NOTE_D0, 
NOTE_D1,NOTE_D1,NOTE_D3,  
NOTE_D1,NOTE_D1,NOTE_D3,NOTE_D0,  
NOTE_D6,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D6, 
NOTE_D5,NOTE_D1,NOTE_D3,NOTE_D0,  
NOTE_DH1,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D6, 
NOTE_D5,NOTE_D1,NOTE_D2,NOTE_D0, 
NOTE_D7,NOTE_D7,NOTE_D5,NOTE_D3, 
NOTE_D5,  
NOTE_DH1,NOTE_D0,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D5,NOTE_D6,NOTE_D6, 
NOTE_D0,NOTE_D5,NOTE_D1,NOTE_D3,NOTE_D0,  
NOTE_DH1,NOTE_D0,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D5,NOTE_D6,NOTE_D6, 
NOTE_D0,NOTE_D5,NOTE_D1,NOTE_D2,NOTE_D0, 
NOTE_D3,NOTE_D3,NOTE_D1,NOTE_DL6, 
NOTE_D1,  
NOTE_D3,NOTE_D5,NOTE_D6,NOTE_D6, 
NOTE_D3,NOTE_D5,NOTE_D6,NOTE_D6, 
NOTE_DH1,NOTE_D0,NOTE_D7,NOTE_D5, 
NOTE_D6,  
};//这部分就是整首曲子的音符部分，用了一个序列定义为tune，整数
float noteDurations[] = { 
1,1,0.5,0.5,1,  
0.5,0.5,0.5,0.5,1,0.5,0.5, 
0.5,1,0.5,1,0.5,0.5, 
0.5,0.5,0.5,0.5,1,1, 
1,1,1+1,  
0.5,1,1+0.5,1, 
1,1,0.5,0.5,1, 
0.5,1,1+0.5,1,  
0.5,0.5,0.5,0.5,1+1, 
0.5,1,1+0.5,1, 
1+1,0.5,0.5,1, 
1+1+1+1,  
0.5,0.5,0.5+0.25,0.25,0.5+0.25,0.25,0.5+0.25,0.25, 
0.5,1,0.5,1,1,  
0.5,0.5,0.5+0.25,0.25,0.5+0.25,0.25,0.5+0.25,0.25, 
0.5,1,0.5,1,1, 
1+1,0.5,0.5,1, 
1+1+1+1, 
0.5,1,0.5,1+1, 
0.5,1,0.5,1+1, 
1+1,0.5,0.5,1,  
1+1+1+1, 
};//这部分是整首曲子的接拍部分，也定义个序列duration，浮点（数组的个数和前面音符的个数是一样的，一一对应么）

// 演奏乐曲
void play()
{
    for (int thisNote = 0; thisNote < lengths; thisNote++) 
    {
    tone(buzzer, melody[thisNote]);
    delay(400*noteDurations[thisNote]);
    noTone(buzzer);
    analogWrite(redpin,  melody[thisNote]%256);
    analogWrite(greenpin,melody[thisNote-1]%256);
    analogWrite(bluepin, melody[thisNote+1]%256);
   }
   //delay(5000);
}

// 实例化一个名为lcd的LiquidCrysta类的对象，并初始化相关引脚
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// 时间相关变量
int year1;
int year;
int month;
int day;
int hour;
int minute;
int second;

// EEPROM的存储地址 
int addr;
unsigned long temp1 = 0;
unsigned long temp2;
int temp = 0;
int val;//定义变量
int dat;//定义变量
int c;

void setup()
{
  //pinMode(buzzer,OUTPUT);//设置数字IO脚模式，OUTPUT为输出
    Serial.begin(9600);//设置波特率
    pinMode(TrigPin,OUTPUT);
    pinMode(EchoPin, INPUT);
    pinMode(redpin, OUTPUT);
    pinMode(bluepin, OUTPUT);
    pinMode(greenpin, OUTPUT);
    lengths=sizeof(melody)/sizeof(melody[0]);

  // 设置LCD有几行几列，1602LCD为16列2行
  lcd.begin(16, 2);

  // 读取EEPROM中的时间值
  hour = EEPROM.read(addr);
  ++addr;
  minute = EEPROM.read(addr);
  ++addr;
  second = EEPROM.read(addr);
  ++addr;
  year1 = EEPROM.read(addr);
  ++addr;
  year = EEPROM.read(addr);
  ++addr;
  month = EEPROM.read(addr); 
  ++addr;
  day = EEPROM.read(addr);
  // 向LCD屏写入时间
  lcd.setCursor(0, 1);    // 定位光标位置，下同
  if (hour > 9)
    lcd.print( hour);
  else
  {
    lcd.print("0");
    lcd.print(hour);
  }
  lcd.print(":");
  if (minute > 9)
    lcd.print( minute);
  else
  {
    lcd.print("0");
    lcd.print(minute);
  }
  lcd.print(":");
  if (second > 9)
    lcd.print( second);
  else
  {
    lcd.print("0");
    lcd.print(second);
  }
  lcd.print(" ");
  addr = 0;
  distance=100;
  
}

void loop()
{
  //写入彩灯引脚值
  analogWrite(redpin, 255);
  analogWrite(greenpin, 255);
  analogWrite(bluepin, 255);
 lcd.setCursor(0, 0);
  if (year1 > 9)
  lcd.print(year1);
  else
  {
    lcd.print("0");
    lcd.print(year1);
  }
  if (year > 9)
  lcd.print(year);
  else
  {
    lcd.print("0");
    lcd.print(year);
  }
  lcd.print("-");
  if (month > 9)
    lcd.print( month);
  else
  {
    lcd.print("0");
    lcd.print(month);
  }
  lcd.print("-");
  if (day > 9)
  lcd.print(day);
  else
  {
    lcd.print("0");
    lcd.print(day);
  }
  lcd.print(" ");

  // 以下使用调用系统时间函数来计算时间，保证计时准确性
  ++temp;
  if (temp >= 31)
  {
    ++second;
    temp = 0;
  }
  if (second == 60)
  {
    second = 0;
    ++minute;
      if (minute == 60)
      {
        minute = 0;
        ++hour;
        if (hour == 24)
        {
         hour = 0;
         ++day;
        }
        
      }
      
  }

  // 刷新时间
  lcd.setCursor(0, 1);
  if (hour > 9)
         lcd.print(hour);
        else
        {
           lcd.print("0");
           lcd.print(hour);
         }
 lcd.print(":");
      if (minute > 9)
       lcd.print( minute);
       else
       {
         lcd.print("0");
         lcd.print(minute);
       }
lcd.print(":");
  if (second > 9)
         lcd.print(second);
        else
        {
           lcd.print("0");
           lcd.print(second);
         }
  lcd.print(" ");
  
      //显示温度部分
      val=analogRead(0);// 读取传感器的模拟值并赋值给val
      dat=(125*val)>>8;//温度计算公式
      lcd.setCursor(11, 0);
      lcd.print(dat);
      lcd.print("C ");
      lcd.print("    ");
    
      // 串口通信
    while(Serial.available())
    {
      char val=Serial.read(); 
      // 获取当前温度显示到串口
      if(val=='t')
      {
        Serial.print("Tep:");//原样输出显示Tep 字符串代表温度
        Serial.print(dat);//输出显示dat 的值
        Serial.println("C");//原样输出显示C 字符串
      }

      // 修改时间
      if (val == 'a')
      {
        Serial.println("Ajust time");
  // 读取格式化输入的时间并将变量进行相应赋值
        c = Serial.read();
        c -= 48;
        Serial.println(c);
        int d = Serial.read();
        Serial.println(d - 48);
        year1 = c * 10 + d - 48;
        delay(100);
        
        c = Serial.read();
        c -= 48;
        Serial.println(c);
        d = Serial.read();
        Serial.println(d - 48);
        year = c * 10 +d - 48;
        delay(100);
        
        Serial.read();
        c = Serial.read();
        c -= 48;
        Serial.println(c);
        d = Serial.read();
        Serial.println(d - 48);
        month = c * 10 + d - 48;
        delay(100);
                 
        Serial.read();
        c = Serial.read();
        c -= 48;
        Serial.println(c);
        d = Serial.read();
        Serial.println(d - 48);
        day = c * 10 + d - 48;
        delay(100);
                 
        Serial.read();
        c = Serial.read();
        c -= 48;
        Serial.println(c);
        d = Serial.read();
        Serial.println(d - 48);
        hour = c * 10 + d - 48; 
        delay(100); 
                
          Serial.read();
          c = Serial.read();
          c -= 48;
          Serial.println(c);
          d = Serial.read();
          Serial.println(d - 48);
          minute = c * 10 + d - 48; 
          delay(100);
                
          Serial.read();
          c = Serial.read();
          c -= 48;
          Serial.println(c);
          d = Serial.read();
          Serial.println(d - 48);
          second = c * 10 + d - 48;
        }
      // 清空缓冲区
      while (Serial.available() > 0)
         Serial.read();
    }   

    // 距离每2秒刷新一次
    if (temp == 0 && second % 2 == 0)
    {
  // 超声波测距
    digitalWrite(TrigPin,LOW);
    delayMicroseconds(2);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin,LOW);
    distance = pulseIn(EchoPin, HIGH) / 58.00;
     lcd.setCursor(9, 1);
     lcd.print(distance);
     lcd.print("cm");
     lcd.print("  ");
    }

    // 当距离过小或温度过高时警报
    if(distance<0)
    {
      
      while(1)
      {
        play();
      }
    }
    if(dat>30)
    {
      while(1)
      {
        play();
      }
    }
  
    //将当前时间写入EEPROM
  EEPROM.write(addr, hour);
  ++addr;
  EEPROM.write(addr,  minute);
  ++addr;
  EEPROM.write(addr, second);
  ++addr;
  EEPROM.write(addr, year1);
  ++addr;
  EEPROM.write(addr, year);
  ++addr;
  EEPROM.write(addr, month);
  ++addr;
  EEPROM.write(addr, day);
  addr = 0;
}

