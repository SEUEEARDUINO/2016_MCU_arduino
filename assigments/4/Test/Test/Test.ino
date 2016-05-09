/*************************实验一************
 * 作者：吴政，16014210
*******************************************/
#include <EEPROM.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(4,5,6,7,8,9);
int my_time[6];
char my_buffer[20];
int Trig=11;
int Echo=12;
int state=0;//状态参数，用于决定是测距模块工作还温度模块工作
float temp;//温度信号接收端口
float distance;//距离信号
//*********时间显示函数*******
void Time_print(){
  my_time[5]++;
  if(my_time[5]==60) {my_time[5]=0;my_time[4]++;}
  if(my_time[4]==60) {my_time[4]=0;my_time[3]++;}
  if(my_time[3]==24) {my_time[3]=0;my_time[2]++;}//设置时间
  //显示时间
  lcd.setCursor(0,2);//设置光标
  for(int i=0;i<2;i++){lcd.print(my_time[i]);lcd.print('/');}
  lcd.print(my_time[2]);lcd.print(' ');
  for(int i=3;i<5;i++){
    if(my_time[i]<10)
    lcd.print(0);
    lcd.print(my_time[i]);
    lcd.print(':');
    }
   if(my_time[5]<10) lcd.print(0);
   lcd.print(my_time[5]);
  
  for(int i=0;i<6;i++) EEPROM.write(i,my_time[i]);//存入eepram中
  }
  //******温度函数*******
  void Temp_print(){
    distance=20;
    temp=analogRead(A0)/1023.0*5000/10;//1摄氏度为1mV
    lcd.home();
    lcd.print("Temp:");
    lcd.print(temp);
    lcd.print("^C");
    Ring();//超温报警
   }
   //******蜂鸣器函数*****
   void Ring(){
    if(temp>30||distance>30||distance<5)digitalWrite(10,LOW);
    else digitalWrite(10,HIGH);
   }
   //******测距函数******
   void Distance_print(){
    temp=0;
    digitalWrite(Trig, LOW); 
    delayMicroseconds(2); 
    digitalWrite(Trig, HIGH); 
    delayMicroseconds(10);
    // 检测脉冲宽度，并计算出距离
    distance = pulseIn(Echo, HIGH) / 58.00;
    lcd.home();
    lcd.print("Dist:");
    lcd.print(distance);
    lcd.print("cm");
    Ring();
    }
  //******输入时间转换函数********
  void Exchange(){
    for(int i=0,j=3;i<6;i++,j=j+3){
    my_time[i]=(int(my_buffer[j])-48)*10+int(my_buffer[j+1])-48;
    }
   }
  //**********初始化函数********
void setup(){
  Serial.begin(9600);
  Serial.print("    Set right time like ' $2016/05/08/09:25:10 '  " );
  Serial.println( "   You must strat with $" );
  Serial.println("    Input D to measure distance");
  Serial.println("    Input T to measure temperature");
  for(int i=0;i<6;i++) my_time[i]=EEPROM.read(i);//获得存储器中的时间
  lcd.begin(16,2);//设置显示方式
  lcd.clear();
  pinMode(A0,INPUT);//温度参数端口
  pinMode(10,OUTPUT);//蜂鸣器参数端
  digitalWrite(10,HIGH);
  pinMode(Trig,OUTPUT);
  pinMode(Echo,INPUT);
  }
  //**********循环函数********
  void loop(){
    int num=Serial.available();//输入的字节数
    Serial.readBytes(my_buffer,num);
    if(my_buffer[0]=='D'){state=1;lcd.clear();}//输入的字符为距离就显示距离
    if(my_buffer[0]=='T'){state=0;lcd.clear();}//输入的字符为温度就显示温度
    
    if(num==20&&my_buffer[0]=='$') Exchange();
    Time_print();

    if(state==0) Temp_print();//写两段的意义使得测得的数据0.5s更新一次
    if(state==1) Distance_print();
    delay(500);
    if(state==0) Temp_print();
    if(state==1) Distance_print();
    delay(500);
    
    }
