/*
_____________Светильник для рабочего места_________________
Индикатор(R = D3; G = D6; B = D5)
Основной свет(R = D11; B = D10; G = D9)
Джойстик(SW = D2; X = A0; Y = A1)
*/
#include <Arduino.h>
int indication_R = 3; //свет индикатора
int indication_G = 6; //свет идикатора
int indication_B = 5; //свет идикатора
int general_led_R = 11; //основной свет
int general_led_B = 10; //основной свет
int general_led_G = 9; //основной свет
int SW = 2; //кнопка
int X = A1; //ось Х
int Y = A2; //ось Y
uint8_t led_case; //переменная для switch
bool last_button = false; //старое состояние кнопки
bool enable_general_led = true; //состояние основного света

int LED(int r, int g, int b, bool enable) { //функция для зажигания света
  analogWrite(indication_R, r); //свет индикатора горит постоянно
  analogWrite(indication_G, g);
  analogWrite(indication_B, b);
  if(enable == true) { //если состояние кнопки как true то зажигаем основной свет
    analogWrite(general_led_R, r);
    analogWrite(general_led_G, g);
    analogWrite(general_led_B, b);
  }else{//если состояние как false то выключаем основной свет
    analogWrite(general_led_R, 0);
    analogWrite(general_led_G, 0);
    analogWrite(general_led_B, 0);
  }//if(enable == true)
}//int LED(int r, int g, int b, bool enable)

void setup() {
  pinMode(general_led_R, OUTPUT); //определяем состояние пинов
  pinMode(general_led_B, OUTPUT);
  pinMode(general_led_G, OUTPUT);
  pinMode(indication_B, OUTPUT);
  pinMode(indication_G, OUTPUT);
  pinMode(indication_R, OUTPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(X, INPUT);
  pinMode(Y, INPUT);
}//setup

bool bounce() { //функция для подавления дребезга
  bool current = digitalRead(SW); //читаем состояние кнопки
  if(current != last_button) { //если состояние кнопки не соотвествует изначальному состоянию то ждем 10 мс и записываем новое состояние в current
    delay(10);
    current = digitalRead(SW);
  }//if(current != last_button)
  return current; //возвращаем новое состояние кнопки
}//bool bounce

void loop() {
  int r_val = 0; //изначальное состояние красного
  int g_val = 0; //изначальное состояние зеленого
  int b_val = 0; //изнчальное состояние синего
  int data_X = analogRead(X); //переменная для снятия сырых данных с оси X
  int data_Y = analogRead(Y); //переменная для снятия сырых данных с оси Y
  int val_X = map(data_X, 0, 1024, 0, 3); //ограничиваем ось X интервалом от 0 до 2
  int val_Y = map(data_Y, 0, 1024, 0, 3); //ограничиваем ось Y интервалом от 0 до 2
  if(val_Y == 2) {//если val_Y равен 2
    delay(500);//то ждем 500 мс
    if(val_Y == 2) {//проверяем снова val_Y
      led_case++;//увеличиваем переменную для switch на 1 для перехода в кейс
      led_case = min(led_case, 4);//ограничиваем переменную максимальным колличеством кейсом в switch
    }//if(val_Y == 2)
  }else if(val_Y == 0) {//если val_Y равен 0
    delay(500);//то ждем 500 мс
    if(val_Y == 0) {//проверяем снова val_Y
      if(led_case != 0) {//ограничиваем переменную минимальным количеством кейсов в switch
        led_case--;//уменьшаем переменную для switch на 1 для перехода в предыдущий кейс
      }//if(led_case != 0)
    }//if(val_Y == 0)
  }//else if(val_Y == 0)

  int current_button = bounce();//принимаем новое состояние кнопки из функции bounce
  if(last_button == LOW && current_button == HIGH) {//если изначальное состояние false, а новое true то
    enable_general_led = !enable_general_led;//инвертируем переменную для включения основного света
  }//if(last_button == LOW && current_button == HIGH)
  last_button = current_button; //передаем новое состояние как старое состояние кнопки после нажатия

  switch (led_case) {//режимы цвета
    case 0://белый
    r_val = 255;
    g_val = 255;
    b_val = 255;
    break;

    case 1://фиолетовый
    r_val = 62;
    g_val = 6;
    b_val = 148;
    break;

    case 2://бирюзовый
    r_val = 48;
    g_val = 213;
    b_val = 200;
    break;

    case 3://желтый
    r_val = 229;
    g_val = 190;
    b_val = 1;
    break;

    case 4:
    r_val = 250;
    g_val = 238;
    b_val = 221;
    break;
  }//switch(led_case)

  LED(r_val, g_val, b_val, enable_general_led); //передаем значения цветов и состояние кнопки в функцию включения основного света
}//loop
