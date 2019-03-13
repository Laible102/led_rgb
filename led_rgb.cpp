/*
  _____________Светильник для рабочего места_________________
  Индикатор(R = D3; G = D6; B = D5)
  Основной свет(R = D11; B = D10; G = D9)
  Джойстик(SW = D2; X = A0; Y = A1)
*/
#include <Arduino.h>


// определение пинов как переменных - не нужные тратры памяти контроллера.
//Пины кошерно определять через дефайны
/*
 #define INDICATON_R 3
 #define INDICATON_G 6
 и так далее
 */
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



#define DEADZONE 100 // мертвая зона в середине джойстика чтобы не было реакции на дребезг
// uint8_t это аналог byte
// uint32_t аналог unsigned long
uint8_t yState = 0; // автомат опроса джойстика
uint8_t yCount = 0; // счетчик фиксаций джойстика
uint32_t ms = 0, yMs = 0; // переменные для отсчетов времени

#define DEBUG // для  отладки




int LED(int r, int g, int b, bool enable) { //функция для зажигания света
  /*
    эти три строки не несут никакого смысла. все исполняется в словиях проверки enable
    analogWrite(indication_R, r); //свет индикатора горит постоянно
    analogWrite(indication_G, g);
    analogWrite(indication_B, b);
  */
  if (enable == true) { //если состояние кнопки как true то зажигаем основной свет
    analogWrite(general_led_R, r);
    analogWrite(general_led_G, g);
    analogWrite(general_led_B, b);
  } else { //если состояние как false то выключаем основной свет
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
  Serial.begin(115200);
}//setup

bool bounce() { //функция для подавления дребезга
  bool current = digitalRead(SW); //читаем состояние кнопки
  if (current != last_button) { //если состояние кнопки не соотвествует изначальному состоянию то ждем 10 мс и записываем новое состояние в current
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
  ms = millis();

  //  опрос по вертикали
  switch (yState) {
    // инициализация автомата опроса
    case 0:
      yMs = 0; // для временного интервала
      yCount = 0; // счетчик фиксаций отклоненного джойтсика
      yState = 5; // GO
      break;
    //ожидание поднятия или опускания джойстика джойстика
    case 5:
      // опрашиваем каждых 20 мс
      if ((ms - yMs) > 10) {
        yMs = ms;
        // если джойстик UP
        if (data_Y > 512 + DEADZONE) {
          yCount++;
          if (yCount > 8) {
            // здесь получена инфа, что джойстик подняли вверх
            // делаем свои дела и уходим на ожидание опускания джойстика
            if (led_case < 4) led_case++;//увеличиваем переменную для switch на 1 для перехода в кейс
            yCount = 0;
            yState = 7; // GO
#ifdef DEBUG
            //F() переносит текст печати из оперативы ( которой у нас очень мало) в память программы.
            //это позвояет  не выхватить stackoverflow
            // и больше места появляется под печать, которая своим печатным текстом жрет память, как дурная
            Serial.print(F("UP "));
            Serial.println(led_case);
#endif
          }//yCount > 10
        } // y >> UP
        // если джойстик DOWN
        else if (data_Y < 512 - DEADZONE) {
          yCount++;
          if (yCount > 8) {
            // здесь получена инфа, что джойстик опущен
            // делаем свои дела и уходим на ожидание опускания джойстика
            if (led_case != 0) led_case--;//уменьшаем переменную для switch на 1 для перехода в предыдущий кейс
            yCount = 0;
            yState = 7; // GO
#ifdef DEBUG
            Serial.print(F("DOWN "));
            Serial.println(led_case);
#endif
          }//yCount > 10
        }// y >> DOWM
        // в пограничных состояниях - дребезг, обнуляем счетчик
        else yCount = 0;
      }//if ms
      break;
    // ожидание опускания джойстика на начальное положение
    case 7:
      if ((ms - yMs) > 10) {
        yMs = ms;
        // если джойстик UP
        if ((data_Y < 512 + DEADZONE) && (data_Y > 512 - DEADZONE)) {
          yCount++;
          if (yCount > 8) {
            // джойстик вернулся
            yCount = 0;
            yState = 0; // GO на исходную
#ifdef DEBUG
            Serial.println(F("RELASED"));
#endif
          }// yCount > 10
        } //data_Y
        // обязательно сброс счетчика на пограничных состояниях
        else yCount = 0;
      }// ms
      break;
  }//switch( y_state){

  //
  //****** для кнопки я бы аналогичный свичик сделал. дебонс отъедает лишний таймер, который в будущем бы пригодился *****
  //
  int current_button = bounce();//принимаем новое состояние кнопки из функции bounce
  if (last_button == LOW && current_button == HIGH) { //если изначальное состояние false, а новое true то
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
