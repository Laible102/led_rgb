/*
_____________Светильник для рабочего места_________________
 Основной свет(R = D11; B = D10; G = D9)
 Джойстик(SW = D2; X = A0; Y = A1)
 */
#include <Arduino.h>
#define deadzone 100
#define DEBUG
#define general_led_R 11
#define general_led_G 9
#define general_led_B 10
#define pinA_encoder 3
#define pinB_encoder 4
#define SW 2
#define X A1
#define Y A2

const byte dim_curve[] = {
  0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
  3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
  4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
  6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
  8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
  11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
  15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
  20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
  27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
  36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
  48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
  63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
  83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
  110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
  146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
  193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

unsigned int hue = 0;
int saturation = 0;
uint8_t brightness = 0;
int rgb_colors [3];
int fadeVal = 0;
int fadeSpeed = 4;
uint8_t yState = 0, yCount = 0;
uint8_t xState = 0, xCount = 0;
bool last_button = false;
bool enable_general_led = false;
uint32_t ms = 0, yMs = 0, xMs = 0;
unsigned long current_time, loop_time;
unsigned char encoder_a, encoder_b, encoder_a_prev = 0;

void encoder() {
  current_time = millis();
  if(current_time >= (loop_time + 5)) {
    encoder_a = digitalRead(pinA_encoder);
    encoder_b = digitalRead(pinB_encoder);
    if((!encoder_a) && (encoder_a_prev)) {
      if(encoder_b) {
        if (hue <= 359) hue++;
        if (hue == 360) hue = 0;
      }
      else {
        if (hue >= 0) hue--;
        if (hue == 65535) hue = 360;
      }
    }
    encoder_a_prev = encoder_a;
  }
}

void joystick(int data_y, int data_x) {
  ms = millis();

  switch (yState) {
  case 0:
    yMs = 0;
    yCount = 0;
    yState = 5;
    break;

  case 5:
    if((ms - yMs) > 10) {
      yMs = ms;
      if(data_y > 512 + deadzone) {
        yCount++;
        if(yCount > 8) {
          if(saturation <= 254) saturation++;
          yCount = 0;
          yState = 0;
#ifdef DEBUG
          Serial.print(F("UP "));
          Serial.println("saturation = ");
          Serial.println(saturation);
#endif

        }
      }

      else if(data_y < 512 - deadzone) {
        yCount++;
        if (yCount > 8) {
          if(saturation != 0) saturation--;
          yCount = 0;
          yState = 0;
#ifdef DEBUG
          Serial.print(F("DOWN "));
          Serial.println("saturation = ");
          Serial.println(saturation);
#endif
        }
      }

      else yCount = 0;
    }
    break;
  }

  switch (xState) {
  case 0:
    xMs = 0;
    xCount = 0;
    xState = 5;
    break;

  case 5:
    if((ms - xMs) > 10) {
      xMs = ms;
      if(data_x > 512 + deadzone) {
        xCount++;
        if(xCount > 8) {
          if(brightness <= 254) brightness++;
          xCount = 0;
          xState = 0;
#ifdef DEBUG
          Serial.print(F("UP "));
          Serial.println("brightness = ");
          Serial.println(brightness);
#endif

        }
      }

      else if(data_x < 512 - deadzone) {
        xCount++;
        if (xCount > 8) {
          if(brightness != 0) brightness--;
          xCount = 0;
          xState = 0;
#ifdef DEBUG
          Serial.print(F("DOWN "));
          Serial.println("brightness = ");
          Serial.println(brightness);
#endif
        }
      }

      else xCount = 0;
    }
    break;
  }

}

void getRGB(int hue, int sat, int val, int colors[3]) {
  val = dim_curve [val];
  sat = 255 - dim_curve [255-sat];

  int r;
  int g;
  int b;
  int base;

  if(sat == 0) {
    colors[0] = val;
    colors[1] = val;
    colors[2] = val;
  }
  else {
    base = ((255 - sat) * val)>>8;

    switch(hue/60) {
    case 0:
      r = val;
      g = (((val - base) * hue)/60) + base;
      b = base;
      break;

    case 1:
      r = (((val - base) * (60 -(hue%60)))/60)+base;
      g = val;
      b = base;
      break;

    case 2:
      r = base;
      g = val;
      b = (((val - base) * (hue%60))/60)+base;
      break;

    case 3:
      r = base;
      g = (((val - base) * (60-(hue%60)))/60)+base;
      b = val;
      break;

    case 4:
      r = (((val - base) * (hue%60))/60) + base;
      g = base;
      b = val;
      break;

    case 5:
      r = val;
      g = base;
      b = (((val-base) * (60-(hue%60)))/60) + base;
      break;
    }

    colors[0] = r;
    colors[1] = g;
    colors[2] = b;
  }
}

bool bounce() {
  bool current = digitalRead(SW);
  if (current != last_button) {
    delay(10);
    current = digitalRead(SW);
  }
  return current;
}

void setup(){
  Serial.begin(115200);
  pinMode(general_led_R, OUTPUT); //определяем состояние пинов
  pinMode(general_led_B, OUTPUT);
  pinMode(general_led_G, OUTPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(X, INPUT);
  pinMode(Y, INPUT);
}

void loop() {
  bool sw_status = digitalRead(SW);
  int data_X = analogRead(X);
  int data_Y = analogRead(Y);
  fadeVal = fadeVal + fadeSpeed;
  fadeVal = constrain(fadeVal, 0, 255);

  if(fadeVal==255 || fadeVal==0) {
    fadeSpeed =- fadeSpeed;
  }

  int current_button = bounce();
  if (last_button == LOW && current_button == HIGH) {
    enable_general_led = !enable_general_led;
  }
  last_button = current_button;

  if(enable_general_led == false && brightness != 0) {
    analogWrite(general_led_R, rgb_colors[0]);
    analogWrite(general_led_G, rgb_colors[1]);
    analogWrite(general_led_B, rgb_colors[2]);
  }
  else {
    analogWrite(general_led_R, 0);
    analogWrite(general_led_G, 0);
    analogWrite(general_led_B, 0);
  }
  encoder();
  getRGB(hue, saturation, brightness, rgb_colors);
  joystick(data_Y, data_X);
}
