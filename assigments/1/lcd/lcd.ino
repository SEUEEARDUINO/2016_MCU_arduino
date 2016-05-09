#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <DateTime.h>
#include "pitches.h"
//时间显示变量定义
LiquidCrystal lcd(2,3,4,5,6,7);    //lcd分配管脚
unsigned long prevtime;		          //先前时间状态

//温度显示变量定义
unsigned long previousMillis = 0;  //先前毫秒数
int counter=0;					           //计数器
float sum=0.0;                     //用于温度总和计算
float temperature = 0;			       //存储温度
const long interval = 100; 		     //时间间隔
unsigned long currentMillis;       //当前毫秒数
byte degree[8] = {                 //自定义字符“ ° ”
	B00110,
	B01001,
	B01001,
	B00110,
	B00000,
	B00000,
	B00000,
	B00000,
};

//时间存储变量定义
int date[6];						            //存储串口输入的时间
byte save[5];                       //保存时间至EEPROM
int copyYear;						
int addr = 0;						            //ＥＥＰＲＯＭ地址

//声音变量定义
const int SPEAKER=8;        	    	//Speaker on Pin 9

//超声波测距变量定义
const int echopin = 13; 			      // echo接3端口
const int trigpin = 12; 		      	// trig接2端口
float distance;						          //超声波测得距离

void setup(){
	lcd.begin(16,2);
  readData();
	// Replace this with the most current date
	DateTime.sync(DateTime.makeTime(save[0], save[1], save[2], save[3], save[4], copyYear)); // sec, min, hour, date, month, year 
	Serial.begin(9600);
	delay(1000);
	lcd.createChar(0,degree);
	pinMode(echopin,INPUT);         //设定echo为输入模式
	pinMode(trigpin,OUTPUT);        //设定trig为输出模式
  pinMode(SPEAKER,OUTPUT);        //设定SPEARKER为输出模式
	temperature = count();				 //计算温度
	showTemperature();					   //显示温度
}

void loop(){    
    if(DateTime.available()) {
        prevtime = DateTime.now();
        while( prevtime == DateTime.now() ){// wait for the second to rollover 
          countTemperature();   		        //多次计算温度取平均值
          distance = countDistance();	      //超声波测距
          judge();						              //判断温度或距离是否超过标准，发出警报
        }
        DateTime.available(); 			        //refresh the Date and date properties
        digitalClockDisplay(); 			        // update digital clock
        saveData();
    }
    showTemperature();
    interact();							                //与上位机交互
}

void printDigits(byte digits){
	// utility function for digital clock display: prints preceding colon and leading 0
	lcd.print(":");
	if(digits < 10)
        lcd.print('0');
	lcd.print(digits,DEC);
}

//lcd显示时间、日期
void digitalClockDisplay(){
  
	lcd.setCursor(3,0);
  
	lcd.print(DateTime.Year);
	lcd.print("/");

	if(DateTime.Month <10)
        lcd.print('0');
	lcd.print(DateTime.Month,DEC);
	lcd.print("/");
  
	if(DateTime.Day <10)
        lcd.print('0');
	lcd.print(DateTime.Day,DEC);
  
	//lcd.print(" ");
	lcd.setCursor(0,1);
	if(DateTime.Hour <10)      
        lcd.print('0');
	// digital clock display of current time
	lcd.print(DateTime.Hour,DEC);
	printDigits(DateTime.Minute);
	printDigits(DateTime.Second);
}

//存储时间至EEPROM
void saveData(){
	save[0] = DateTime.Second;
	save[1] = DateTime.Minute;
	save[2] = DateTime.Hour;
	save[3] = DateTime.Day;
	save[4] = DateTime.Month;
	for(addr = 0;addr < 5;addr++)
        EEPROM.put(addr,save[addr]);
	EEPROM.put(addr,DateTime.Year);
}

//从ＥＥＰＲＯＭ读取时间数据
void readData(){
	for(addr = 0;addr < 5;addr++)
        EEPROM.get(addr,save[addr]);
	EEPROM.get(addr,copyYear);
}

//计算温度
void countTemperature(){
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; 
        sum+=count();  				//使用浮点数存储温度数据，温度数据由电压值换算得到
        counter++;
        if(counter==10){
            counter=0;
            temperature = sum/10;
            sum=0;
        } 
    }
}

//计算一次温度
float count(){
	int n = analogRead(A5);    		//读取A5口的电压值
	float vol = n * (5.0 / 1023.0*100); 
	return vol;
}
void showTemperature(){
    lcd.setCursor(9,1);
    lcd.print(temperature); 		//输出温度数据 
    lcd.setCursor(14,1);
    lcd.write((byte)0);
    lcd.setCursor(15,1);
    lcd.print("C");
}
float countDistance(){
	float d;
	digitalWrite(trigpin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigpin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigpin,LOW); 		//发一个10ms的高脉冲去触发TrigPin
    d = pulseIn(echopin,HIGH);		//接收高电平时间
    d= d/58.0;						        //计算距离
    delay(10);   				        	//循环间隔10mS
	return ( d>0 ? d : 100);
}

//与上位机交互
void interact(){
	int i;
	long a = 0;
	long b = 0;
	for(i=0;i<4;i++)
		if(Serial.available()> 0){
          
			b *= 10;
			b += (Serial.read()-48);
			delay(4);       
		}
	while(Serial.available()> 0){
		a *= 10;
		a += (Serial.read()-48);
		delay(4);     
	}
	//串口温度显示
	if((b == 1)&&temperature){
        Serial.print("temperature: ");
        Serial.println(temperature);
	}
	else if(a > 2016){
        for(i=0;i < 5;i++){
            date[i] = a%100;
            a /= 100;
        }
        date[5] = b;
        DateTime.sync(DateTime.makeTime(date[0],date[1], date[2], date[3], date[4], date[5]));
        //串口日期显示
        for(i = 5;i > 3;i--){
            Serial.print(date[i]);
            Serial.print("/");
        }
        Serial.println(date[3]);
        //串口时间显示
        for(i = 2;i > 0;i--){
             Serial.print(date[i]);
             Serial.print(":");
        }
        Serial.println(date[0]);
     }
  
}
//判断温度、时间是否超过标准，发出警报
void judge(){
	if((temperature > 32)||(distance < 30)){
        Serial.print("Distance:");
        Serial.println(distance);
        Serial.print("Temperature:");
        Serial.println(temperature);
        for(int i = 0;i < 20;i++){
			tone(SPEAKER,notes[i],times[i]);
            showTemperature();
            delay(times[i]);            
            DateTime.available(); 		//refresh the Date and date properties
            digitalClockDisplay( ); 	// update digital clock
            saveData();
        } 
    }
    noTone(SPEAKER);
}

  
