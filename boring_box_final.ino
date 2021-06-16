/*项目说明：
新款无聊盒子（闪电猫功夫狗开源项目）
项目成员：黎健锋（代码）、 黄叶波（结构图纸）、
        张瑛娜（IP设计） 胡乙伟（项目跟进）
指导老师：陈翀
江西财经大学 艺术学院《设计方法学》201学期期末作业 
联系方式：QQ——100333829
任何问题欢迎联系
2021.6.15免费开源
*/

#include <Servo.h>            
#include "OneButton.h"      //需要预先安装Onebutton的库

#define PIN_INPUT 2
#define PIN_LED 13      

OneButton button(PIN_INPUT, false, false);    //在2接口使用压力传感器，若为普通按钮则设置为OneButton button(PIN_INPUT, true)
int ledState = LOW;
unsigned long pressStartTime;
int opentest = 1;
int actionstate = 1;

//////////////////////动作库说明///////////////////////////
const int rightnormal[][4] = {       ////////默认六行，后面有代码可以改//////////////
  {4, 70, 15, 100}, //开门右门    //1为LeftArm  ｜  0是缩进去100是伸手出来  ｜20为旋转速度｜500为等待时间
  {4, 20, 15, 200}, //关闭右门     // 2为RightArm｜ 20是缩进去90是伸手出来  ｜ (越低越快) ｜(毫秒)
  {4, 90, 15, 1000}, //开门右门    //3为LeftOpen ｜ 0是关闭 60是打开       ｜          ｜
  {2, 20, 15, 0}, //用右手推开关   //4为RightOpen｜   20是关闭70是打开      ｜          ｜
  {2, 120, 15, 200},
  {4, 20, 15, 200}
};

const int leftangry[][4] = {
  {3, 60, 15, 500},
  {1, 70, 30, 100},
  {1, 60, 15, 100},
  {1, 120, 30, 100},
  {1, 0, 15, 500},
  {3, 0, 15, 200}
};
const int rightangry[][4] = {
  {4, 70, 40, 100},
  {2, 70, 25, 200},
  {2, 90, 15, 1000},
  {2, 20, 15, 0},
  {2, 120, 15, 200},
  {4, 20, 15, 200}
};

const int knock1[][4] = {    //其它动作库
  {3, 60, 20, 300},
  {3, 0, 20, 500},
  {3, 0, 20, 200},
  {3, 0, 20, 200},
  {3, 0, 20, 200},
  {3, 0, 20, 200}
};
const int knock2[][4] = {
  {4, 70, 20, 300},
  {4, 20, 20, 500},
  {4, 0, 15, 200},
  {4, 0, 15, 200}, {4, 0, 15, 200}, {4, 0, 15, 200}
};

const int leftnormal[][4] = {
  {3, 60, 20, 100},
  {3, 0, 20, 200},
  {3, 70, 20, 2000},
  {1, 140, 20, 10},
  {1, 0, 20, 200},
  {3, 0, 20, 200}
};
Servo LeftArm, LeftOpen, RightArm, RightOpen;  //建立4个电机对象

bool btnmode = false;
int LeftArmMax = 180;       //设置安全最大安全角度，防止卡住烧毁电机
int LeftOpenMax = 160;
int RightArmMax = 180;
int RightOpenMax = 100;

int LeftArmMin = 0;
int LeftOpenMin = 0;
int RightArmMin = 0;
int RightOpenMin = 0;
int dataIndex = 0;
int num = 1;
void setup() {
  LeftArm.attach(9);  
  RightArm.attach(10);   
  LeftOpen.attach(6);   
  RightOpen.attach(11);   

  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(PIN_INPUT), checkTicks, CHANGE);
  // link the xxxclick functions to be called on xxxclick event.
  button.attachClick(singleClick);
  button.attachDoubleClick(doubleClick);
  button.attachMultiClick(multiClick);
  button.setPressTicks(1000); // that is the time when LongPressStart is called
  button.attachLongPressStart(pressStart);
  button.attachLongPressStop(pressStop);

  LeftArm.write(0);
  delay(100);
  RightArm.write(120);
  delay(100);
  LeftOpen.write(180);
  delay(100);
  RightOpen.write(20);
  delay(1000);

}

void loop() {
  button.tick();
  if (btnmode == true) {
    testmode();
    //delay(10);
  }

}


////////////////////////Onebutton按钮反馈模块/////////////////////
void checkTicks()
{
  button.tick(); // just call tick() to check the state.

}


void singleClick()
{
  Serial.println("singleClick() detected.");
  num = num + 2;
  if (num > 4) {
    num = 1;
  }
} // singleClick


// this function will be called when the button was pressed 2 times in a short timeframe.
void doubleClick()
{
  Serial.println("doubleClick() detected.");
  if (opentest == 1) {
    action(knock1);
    opentest = 2;
  } else if (opentest == 2) {
    action(knock2);
    opentest = 1;
  }
} // doubleClick


// this function will be called when the button was pressed 2 times in a short timeframe.
void multiClick()
{
  Serial.print("multiClick(");
  Serial.print(button.getNumberClicks());
  Serial.println(") detected.");
  if (button.getNumberClicks() == 3) { //按压3次轮流执行不同的动作

    switch (actionstate) {
        if (actionstate > 4) {
          actionstate = 1;
        }
      case 1:
        action(leftangry);
        actionstate = actionstate + 1;
        break;
      case 2:
        action(rightangry);
        actionstate = actionstate + 1;
        break;

      case 3:

        action(leftnormal);
        action(rightnormal);
        action(leftangry);
        action(rightangry);
        actionstate = actionstate + 1;
        break;


      case 4:

        action(leftangry);

        action(rightangry);
        actionstate = actionstate + 1;
        break;
    }
  }
  if (button.getNumberClicks() == 5) { //按压3次执行rightnormal动作
    Serial.println("action_rightnormal_started");
    action(rightnormal);
  }

  if (button.getNumberClicks() == 4) { //按压4次执行leftnormal动作
    Serial.println("action_leftnormal_started");
    action(leftnormal);

  }
} // multiClick


void pressStart()
{
  Serial.println("pressStart()");
  pressStartTime = millis() - 1000; // as set in setPressTicks()
} // pressStart()


// this function will be called when the button was pressed 2 times in a short timeframe.
void pressStop()
{
  Serial.print("pressStop(");
  Serial.print(millis() - pressStartTime);
  Serial.println(") detected.");
} // pressStop()




void servoCmd(int servoName, int toPos, int servoDelay, int waitTime) { // 电机控制函数

  Servo servo2go;
  int fromPos; //建立变量，存储电机起始运动角度值
  switch (servoName) {
    case 1:         //1为LeftArm
      if (toPos >= LeftArmMin && toPos <= LeftArmMax) {
        servo2go = LeftArm;
        fromPos = LeftArm.read();
        toPos = toPos;
        if (fromPos <= toPos) { //如果“起始角度值”小于“目标角度值”
          for (int i = fromPos; i < toPos; i++) {
            servo2go.write(i);
            delay (servoDelay);
          }
        }  else { //否则“起始角度值”大于“目标角度值”
          for (int i = fromPos; i > toPos; i--) {
            servo2go.write(i);
            delay (servoDelay);
          }
        }
        Serial.println(servo2go.read());
        break;
      } else {
        Serial.println("Servo Value Out Of Limit");
        return;
      }

    case 2:          //2为RightArm
      if (toPos >= RightArmMin && toPos <= RightArmMax) {
        servo2go = RightArm;
        fromPos = (RightArm.read());  // 角度调转
        toPos = toPos;

        if (fromPos <= toPos) { //如果“起始角度值”小于“目标角度值”
          for (int i = fromPos; i < toPos; i++) {
            servo2go.write(i);
            delay (servoDelay);
          }
        }  else { //否则“起始角度值”大于“目标角度值”
          for (int i = fromPos; i > toPos; i--) {
            servo2go.write(i);
            delay (servoDelay);
          }
        }
        Serial.println(servo2go.read());
        break;
      } else {
        Serial.println("Servo Value Out Of Limit");
        return;
      }

    case 3:         //3为LeftOpen
      if (toPos >= LeftOpenMin && toPos <= LeftOpenMax) {
        servo2go = LeftOpen;
        fromPos = (LeftOpen.read());  // 获取当前电机角度值用于“电机运动起始角度值”
        toPos = 180 - toPos;

        if (fromPos <= toPos) { //如果“起始角度值”小于“目标角度值”
          for (int i = fromPos; i < toPos; i++) {
            servo2go.write(i);
            delay (servoDelay);

          }
        }  else { //否则“起始角度值”大于“目标角度值”
          for (int i = fromPos; i > toPos; i--) {
            servo2go.write(i);
            delay (servoDelay);
          }
        }
        Serial.println(servo2go.read());
        break;
      } else {
        Serial.println("Servo Value Out Of Limit");
        return;
      }

    case 4:         //4为RightOpen
      if (toPos >= RightOpenMin && toPos <= RightOpenMax) {
        servo2go = RightOpen;
        fromPos = (RightOpen.read());  // 获取当前电机角度值用于“电机运动起始角度值”

        if (fromPos <= toPos) { //如果“起始角度值”小于“目标角度值”
          for (int i = fromPos; i < toPos; i++) {
            servo2go.write(i);
            delay (servoDelay);
          }
        }  else { //否则“起始角度值”大于“目标角度值”
          for (int i = fromPos; i > toPos; i--) {
            servo2go.write(i);
            delay (servoDelay);
          }
        }
        Serial.println(servo2go.read());
        break;
      } else {
        Serial.println("Servo Value Out Of Limit");
        return;
      }
  }

  delay(waitTime);

}

}
void action(int order[][4]) {   //动作执行函数
  int len = sizeof(order) / sizeof(order[0]);
  Serial.println("len:" + len);
  for (int i = 0; i < 6; i++) { //轮流执行每一个动作
    servoCmd(order[i][0], order[i][1], order[i][2], order[i][3]);
    Serial.println(order[i][0]);
  }
  Serial.println("action_ended");

}
