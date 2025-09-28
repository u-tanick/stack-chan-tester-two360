#include <M5Unified.h>
#include <Avatar.h>
#include <ServoEasing.hpp>  // 表情用はそのまま利用

using namespace m5avatar;

Avatar avatar;
ColorPalette *cp;

// -------------------- サーボ設定 --------------------
#include <ESP32PWM.h>
#include <ServoEasing.hpp>

ServoEasing servoLeft;   // 左タイヤ
ServoEasing servoRight;  // 右タイヤ

// ====== Core2 用ピン設定 ======
#define SERVO_LEFT_PIN 33   // Port.A X
#define SERVO_RIGHT_PIN 32  // Port.A Y

#define SERVO_MIN 500
#define SERVO_MAX 2500
#define SERVO_STOP 90        // 停止位置
#define SERVO_SPEED 30       // 前進/後退速度幅

// -------------------- 動作設定 --------------------
unsigned long actionStartMillis = 0;
unsigned long actionDuration = 5000; // 各動作の時間（ms）
int actionStep = 0;                   // 現在の動作ステップ
bool isTestRunning = false;

// -------------------- Avatar 表情 --------------------
Expression getRandomExpression() {
  int r = random(0, 6);
  switch(r) {
    case 0: return Expression::Angry;
    case 1: return Expression::Doubt;
    case 2: return Expression::Happy;
    case 3: return Expression::Neutral;
    case 4: return Expression::Sad;
    case 5: return Expression::Sleepy;
    default: return Expression::Neutral;
  }
}

// -------------------- 動作関数 --------------------
void stopMotors() {
  servoLeft.write(SERVO_STOP);
  servoRight.write(SERVO_STOP);
  avatar.setSpeechText("停止");
  actionDuration = 3000;
}

void forward() {
  servoLeft.write(SERVO_STOP + SERVO_SPEED);
  servoRight.write(SERVO_STOP - SERVO_SPEED);
  avatar.setSpeechText("前進");
  actionDuration = 7000;
}

void backward() {
  servoLeft.write(SERVO_STOP - SERVO_SPEED);
  servoRight.write(SERVO_STOP + SERVO_SPEED);
  avatar.setSpeechText("後退");
  actionDuration = 7000;
}

void turnRight() {
  servoLeft.write(SERVO_STOP + SERVO_SPEED);
  servoRight.write(SERVO_STOP + SERVO_SPEED);
  avatar.setSpeechText("右旋回");
  actionDuration = 12000;
}

void turnLeft() {
  servoLeft.write(SERVO_STOP - SERVO_SPEED);
  servoRight.write(SERVO_STOP - SERVO_SPEED);
  avatar.setSpeechText("左旋回");
  actionDuration = 12000;
}

void runActionStep(int step) {
  Expression expr = getRandomExpression();
  avatar.setExpression(expr);

  switch(step) {
    case 0: // 前進
      forward();
      break;
    case 1: // 停止
      stopMotors();
      break;
    case 2: // 後退
      backward();
      break;
    case 3: // 停止
      stopMotors();
      break;
    case 4: // 右旋回
      turnRight();
      break;
    case 5: // 停止
      stopMotors();
      break;
    case 6: // 左旋回
      turnLeft();
      break;
    case 7: // 停止
      stopMotors();
      break;
  }

  actionStartMillis = millis();
}

// -------------------- setup --------------------
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Speaker.setVolume(100);

  // Avatar 初期化
  cp = new ColorPalette();
  cp->set(COLOR_PRIMARY, TFT_WHITE);
  cp->set(COLOR_BACKGROUND, TFT_BLACK);
  avatar.setColorPalette(*cp);
  avatar.init(8);
  avatar.setBatteryIcon(false);
  avatar.setSpeechFont(&fonts::lgfxJapanGothicP_12);
  avatar.setExpression(Expression::Neutral);

  // サーボ初期化
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);

  servoLeft.attach(SERVO_LEFT_PIN, SERVO_MIN, SERVO_MAX);
  servoRight.attach(SERVO_RIGHT_PIN, SERVO_MIN, SERVO_MAX);

  servoLeft.write(SERVO_STOP);
  servoRight.write(SERVO_STOP);

  randomSeed(analogRead(0));
}

// -------------------- loop --------------------
void loop() {
  M5.update();

  // Bボタン割り込みで即時停止
  if (M5.BtnB.wasPressed()) {
    isTestRunning = false;
    servoLeft.write(SERVO_STOP);
    servoRight.write(SERVO_STOP);
    avatar.setExpression(Expression::Neutral);
    avatar.setSpeechText("強制停止");
    return;
  }

  // Aボタンでテスト開始
  if (M5.BtnA.wasPressed() && !isTestRunning) {
    isTestRunning = true;
    actionStep = 0;
    runActionStep(actionStep);
  }

  if (!isTestRunning) return;

  // 一定時間経過で次のステップに移行
  if (millis() - actionStartMillis >= actionDuration) {
    actionStep = (actionStep + 1) % 8;
    runActionStep(actionStep);
  }
}
