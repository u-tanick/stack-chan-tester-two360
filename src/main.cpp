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


// #include <Arduino.h>
// #include <M5Unified.h>
// #include <ServoEasing.hpp>
// #include <Avatar.h>

// using namespace m5avatar;

// // ====== Core2 用ピン設定 ======
// #define SERVO_LEFT_PIN 33   // Port.A X
// #define SERVO_RIGHT_PIN 32  // Port.A Y

// // ====== サーボ＆制御パラメータ ======
// const int STOP_VAL = 90;      // 停止位置

// // 動作パラメータ（調整しやすいよう変数化）
// int SPEED = 45;               // 前後・旋回の速度
// int MOVE_TIME = 5000;         // 前進/後退/旋回時間(ms)
// int PAUSE_TIME = 3000;        // 停止時間(ms)

// ServoEasing servoLeft;
// ServoEasing servoRight;

// Avatar avatar;
// ColorPalette *cp;

// bool testRunning = false;
// bool stopRequested = false;

// // ====== 関数宣言 ======
// void runTestSequence();
// void forward();
// void backward();
// void turnRight();
// void turnLeft();
// void stopMotors();
// void delayWithStopCheck(int duration);
// void setupAvatar();
// void randomExpression(const char* action);

// // =================================
// void setup() {
//   auto cfg = M5.config();
//   M5.begin(cfg);

//   // Avatar初期化
//   setupAvatar();

//   // サーボ初期化
//   ESP32PWM::allocateTimer(0);
//   ESP32PWM::allocateTimer(1);

//   servoLeft.setPeriodHertz(50);
//   servoRight.setPeriodHertz(50);

//   servoLeft.attach(SERVO_LEFT_PIN, 500, 2500);
//   servoRight.attach(SERVO_RIGHT_PIN, 500, 2500);

//   servoLeft.setEasingType(EASE_LINEAR);
//   servoRight.setEasingType(EASE_LINEAR);

//   stopMotors();
//   avatar.setSpeechText("Press A to start test");
// }

// // =================================
// void loop() {
//   M5.update();

//   // 強制停止
//   if (M5.BtnB.wasPressed() && testRunning) {
//     stopRequested = true;
//     avatar.setSpeechText("Force Stop");
//     avatar.setExpression(Expression::Angry);
//   }

//   // テスト開始
//   if (M5.BtnA.wasPressed() && !testRunning) {
//     testRunning = true;
//     stopRequested = false;
//     avatar.setExpression(Expression::Neutral);
//     avatar.setSpeechText("Test Running");
//     runTestSequence();
//     testRunning = false;
//     stopMotors();
//     avatar.setExpression(Expression::Neutral);
//     avatar.setSpeechText("Test Finished");
//   }
// }

// // =================================
// void runTestSequence() {
//   if (stopRequested) return;
//   forward();    randomExpression("Forward");    delayWithStopCheck(MOVE_TIME);

//   if (stopRequested) return;
//   stopMotors(); randomExpression("Stop");       delayWithStopCheck(PAUSE_TIME);

//   if (stopRequested) return;
//   backward();   randomExpression("Backward");   delayWithStopCheck(MOVE_TIME);

//   if (stopRequested) return;
//   stopMotors(); randomExpression("Stop");       delayWithStopCheck(PAUSE_TIME);

//   if (stopRequested) return;
//   turnRight();  randomExpression("Turn Right"); delayWithStopCheck(MOVE_TIME);

//   if (stopRequested) return;
//   stopMotors(); randomExpression("Stop");       delayWithStopCheck(PAUSE_TIME);

//   if (stopRequested) return;
//   turnLeft();   randomExpression("Turn Left");  delayWithStopCheck(MOVE_TIME);

//   if (stopRequested) return;
//   stopMotors(); randomExpression("Stop");       delayWithStopCheck(PAUSE_TIME);
// }

// // =================================
// void stopMotors() {
//   servoLeft.startEaseTo(STOP_VAL);
//   servoRight.startEaseTo(STOP_VAL);
// }

// void forward() {
//   servoLeft.startEaseTo(STOP_VAL + SPEED);
//   servoRight.startEaseTo(STOP_VAL - SPEED);
// }

// void backward() {
//   servoLeft.startEaseTo(STOP_VAL - SPEED);
//   servoRight.startEaseTo(STOP_VAL + SPEED);
// }

// void turnRight() {
//   servoLeft.startEaseTo(STOP_VAL + SPEED);
//   servoRight.startEaseTo(STOP_VAL + SPEED);
// }

// void turnLeft() {
//   servoLeft.startEaseTo(STOP_VAL - SPEED);
//   servoRight.startEaseTo(STOP_VAL - SPEED);
// }

// // =================================
// void delayWithStopCheck(int duration) {
//   int elapsed = 0;
//   const int step = 50;
//   while (elapsed < duration) {
//     if (stopRequested) break;
//     delay(step);
//     elapsed += step;
//     M5.update();
//   }
// }

// // =================================
// void setupAvatar() {
//   cp = new ColorPalette();
//   cp->set(COLOR_PRIMARY, TFT_WHITE);
//   cp->set(COLOR_BACKGROUND, TFT_BLACK);

//   avatar.setColorPalette(*cp);
//   avatar.init(8);
//   avatar.setBatteryIcon(false);
//   avatar.setSpeechFont(&fonts::lgfxJapanGothicP_12);
//   avatar.setExpression(Expression::Neutral);
// }

// // =================================
// // 動作ごとにランダム表情
// void randomExpression(const char* action) {
//   avatar.setSpeechText(action);
//   int expr = random(0, 6); // 0～5
//   switch(expr) {
//     case 0: avatar.setExpression(Expression::Angry); break;
//     case 1: avatar.setExpression(Expression::Doubt); break;
//     case 2: avatar.setExpression(Expression::Happy); break;
//     case 3: avatar.setExpression(Expression::Neutral); break;
//     case 4: avatar.setExpression(Expression::Sad); break;
//     case 5: avatar.setExpression(Expression::Sleepy); break;
//     default: avatar.setExpression(Expression::Neutral);
//   }
// }


// #include <Arduino.h>
// #include <M5Unified.h>
// #include <ServoEasing.hpp>
// #include <Avatar.h>

// using namespace m5avatar;

// // ====== Core2 用ピン設定 ======
// #define SERVO_LEFT_PIN 33   // Port.A X
// #define SERVO_RIGHT_PIN 32  // Port.A Y

// // ====== サーボ＆制御パラメータ ======
// const int STOP_VAL = 90;      // 停止位置

// // 動作パラメータ（調整しやすいよう変数化）
// int SPEED = 45;               // 前後・旋回の速度
// int MOVE_TIME = 20000;         // 前進/後退/旋回時間(ms)
// int PAUSE_TIME = 1000;        // 停止時間(ms)

// ServoEasing servoLeft;
// ServoEasing servoRight;

// Avatar avatar;
// ColorPalette *cp;

// bool testRunning = false;
// bool stopRequested = false;

// // ====== 関数宣言 ======
// void runTestSequence();
// void forward();
// void backward();
// void turnRight();
// void turnLeft();
// void stopMotors();
// void delayWithStopCheck(int duration);
// void setupAvatar();
// void randomExpression(const char* action);

// // =================================
// void setup() {
//   auto cfg = M5.config();
//   M5.begin(cfg);

//   // Avatar初期化
//   setupAvatar();

//   // サーボ初期化
//   ESP32PWM::allocateTimer(0);
//   ESP32PWM::allocateTimer(1);

//   servoLeft.setPeriodHertz(50);
//   servoRight.setPeriodHertz(50);

//   servoLeft.attach(SERVO_LEFT_PIN, 500, 2500);
//   servoRight.attach(SERVO_RIGHT_PIN, 500, 2500);

//   servoLeft.setEasingType(EASE_LINEAR);
//   servoRight.setEasingType(EASE_LINEAR);

//   stopMotors();
//   avatar.setSpeechText("Press A to start test");
// }

// // =================================
// void loop() {
//   M5.update();

//   // 強制停止
//   if (M5.BtnB.wasPressed() && testRunning) {
//     stopRequested = true;
//     avatar.setSpeechText("Force Stop");
//     avatar.setExpression(Expression::Angry);
//   }

//   // テスト開始
//   if (M5.BtnA.wasPressed() && !testRunning) {
//     testRunning = true;
//     stopRequested = false;
//     avatar.setExpression(Expression::Neutral);
//     avatar.setSpeechText("Test Running");
//     runTestSequence();
//     testRunning = false;
//     stopMotors();
//     avatar.setExpression(Expression::Neutral);
//     avatar.setSpeechText("Test Finished");
//   }
// }

// // =================================
// void runTestSequence() {
//   if (stopRequested) return;
//   forward();    randomExpression("Forward");    delayWithStopCheck(MOVE_TIME);

//   if (stopRequested) return;
//   stopMotors(); randomExpression("Stop");       delayWithStopCheck(PAUSE_TIME);

//   if (stopRequested) return;
//   backward();   randomExpression("Backward");   delayWithStopCheck(MOVE_TIME);

//   if (stopRequested) return;
//   stopMotors(); randomExpression("Stop");       delayWithStopCheck(PAUSE_TIME);

//   if (stopRequested) return;
//   turnRight();  randomExpression("Turn Right"); delayWithStopCheck(MOVE_TIME);

//   if (stopRequested) return;
//   stopMotors(); randomExpression("Stop");       delayWithStopCheck(PAUSE_TIME);

//   if (stopRequested) return;
//   turnLeft();   randomExpression("Turn Left");  delayWithStopCheck(MOVE_TIME);

//   if (stopRequested) return;
//   stopMotors(); randomExpression("Stop");       delayWithStopCheck(PAUSE_TIME);
// }

// // =================================
// void stopMotors() {
//   servoLeft.startEaseTo(STOP_VAL);
//   servoRight.startEaseTo(STOP_VAL);
// }

// void forward() {
//   servoLeft.startEaseTo(STOP_VAL + SPEED);
//   servoRight.startEaseTo(STOP_VAL - SPEED);
// }

// void backward() {
//   servoLeft.startEaseTo(STOP_VAL - SPEED);
//   servoRight.startEaseTo(STOP_VAL + SPEED);
// }

// void turnRight() {
//   servoLeft.startEaseTo(STOP_VAL + SPEED);
//   servoRight.startEaseTo(STOP_VAL + SPEED);
// }

// void turnLeft() {
//   servoLeft.startEaseTo(STOP_VAL - SPEED);
//   servoRight.startEaseTo(STOP_VAL - SPEED);
// }

// // =================================
// void delayWithStopCheck(int duration) {
//   int elapsed = 0;
//   const int step = 50;
//   while (elapsed < duration) {
//     if (stopRequested) break;
//     delay(step);
//     elapsed += step;
//     M5.update();
//   }
// }

// // =================================
// void setupAvatar() {
//   cp = new ColorPalette();
//   cp->set(COLOR_PRIMARY, TFT_WHITE);
//   cp->set(COLOR_BACKGROUND, TFT_BLACK);

//   avatar.setColorPalette(*cp);
//   avatar.init(8);
//   avatar.setBatteryIcon(false);
//   avatar.setSpeechFont(&fonts::lgfxJapanGothicP_12);
//   avatar.setExpression(Expression::Neutral);
// }

// // =================================
// // 動作ごとにランダム表情
// void randomExpression(const char* action) {
//   avatar.setSpeechText(action);
//   int expr = random(0, 6); // 0～5
//   switch(expr) {
//     case 0: avatar.setExpression(Expression::Angry); break;
//     case 1: avatar.setExpression(Expression::Doubt); break;
//     case 2: avatar.setExpression(Expression::Happy); break;
//     case 3: avatar.setExpression(Expression::Neutral); break;
//     case 4: avatar.setExpression(Expression::Sad); break;
//     case 5: avatar.setExpression(Expression::Sleepy); break;
//     default: avatar.setExpression(Expression::Neutral);
//   }
// }



// // ==================================
// // 全体共通のヘッダファイルのinclude
// #include <Arduino.h>                         // Arduinoフレームワークを使用する場合は必ず必要
// #include <SD.h>                              // SDカードを使うためのライブラリです。
// #include <Update.h>                          // 定義しないとエラーが出るため追加。
// #include <Ticker.h>                          // 定義しないとエラーが出るため追加。
// #include <M5StackUpdater.h>                  // M5Stack SDUpdaterライブラリ
// #include <M5Unified.h>                       // M5Unifiedライブラリ
// // ================================== End

// // ==================================
// // for Avatar
// #include <Avatar.h>                          // 顔を表示するためのライブラリ https://github.com/meganetaaan/m5stack-avatar
// using namespace m5avatar;                    // (Avatar.h)avatarのnamespaceを使う宣言（こうするとm5avatar::???と書かなくて済む。)
// Avatar avatar;                               // (Avatar.h)avatarのクラスを定義
// ColorPalette *cp;                            // アバターのカラーパレット用

// // 表情を変更
// void changeExpression(int num) {
//   switch(num) {
//     case 0:
//       avatar.setExpression(Expression::Angry);
//       break;
//     case 1:
//       avatar.setExpression(Expression::Doubt);
//       break;
//     case 2:
//       avatar.setExpression(Expression::Happy);
//       break;
//     case 3:
//       avatar.setExpression(Expression::Neutral);
//       break;
//     case 4:
//       avatar.setExpression(Expression::Sad);
//       break;
//     case 5:
//       avatar.setExpression(Expression::Sleepy);
//       break;
//     default:
//       avatar.setExpression(Expression::Neutral);
//   }
// }
// // ================================== End

// // ==================================
// // for SystemConfig
// #include <Stackchan_system_config.h>          // stack-chanの初期設定ファイルを扱うライブラリ
// StackchanSystemConfig system_config;          // (Stackchan_system_config.h) プログラム内で使用するパラメータをYAMLから読み込むクラスを定義
// // ================================== End

// // ==================================
// // for SD
// #define SDU_APP_PATH "/stackchan_tester.bin"  // (SDUpdater.h)SDUpdaterで使用する変数
// #define TFCARD_CS_PIN 4                       // SDカードスロットのCSPIN番号
// // ================================== End

// // ==================================
// // for Servo
// #include <ServoEasing.hpp>

// /**
//  * サーボのPWM値と回転方向の関係（最右列は180サーボの場合の角度）
//  * 
//  * Tower Pro
//  * 時計回り   : 500 - 1500US : 0 - 90度
//  * 停止       : 1500US : 90度
//  * 反時計周り : 1500 - 2400US : 90 - 180度
//  * 
//  * Feetech
//  * 時計回り   : 700 - 1500US : 0 - 90度
//  * 停止       : 1500US : 90度
//  * 反時計周り : 1500 - 2300US : 90 - 180度
//  * 
//  * M5Stack
//  * 時計回り   : 500 - 1500US : 0 - 90度
//  * 停止       : 1500US : 90度
//  * 反時計周り : 1500 - 2500US : 90 - 180度
// */

// ServoEasing servo180;  // 180度サーボ
// ServoEasing servo360;  // 360度サーボ

// // 使用する180サーボのメーカー
// // #define USE_Servo_180_TowerPro
// // #define USE_Servo_180_Feetech360
// #define USE_Servo_180_M5Stack

// #define START_DEGREE_VALUE_SERVO_180  85  // 180度サーボ（Y軸方向）の初期角度(全サーボ共通)

// // サーボの種類毎のPWM幅や初期角度、回転速度のレンジ設定など
// #ifdef USE_Servo_180_TowerPro
//   const int MIN_PWM_180 = 500;
//   const int MAX_PWM_180 = 2400;
// #endif
// #ifdef USE_Servo_180_Feetech360
//   const int MIN_PWM_180 = 700;
//   const int MAX_PWM_180 = 2300;
// #endif
// #ifdef USE_Servo_180_M5Stack
//   const int MIN_PWM_180 = 500;
//   const int MAX_PWM_180 = 2500;
// #endif

// // 使用する360サーボのメーカー
// // #define USE_Servo_360_TowerPro
// // #define USE_Servo_360_Feetech360
// #define USE_Servo_360_M5Stack

// // サーボの種類毎のPWM幅や初期角度、回転速度のレンジ設定など
// #ifdef USE_Servo_360_TowerPro
//   const int MIN_PWM_360 = 500;
//   const int MAX_PWM_360 = 2400;
//   // const int START_DEGREE_VALUE_SERVO_360 = 90;     // 360度サーボの停止位置：仕様では90で停止
//   const int START_DEGREE_VALUE_SERVO_360 = 95;        // サーボ個体差で、90度指定で停止しなかった場合値を変えてみる（試作に使用したsg90-hvの場合95付近で停止だった)
//   const int SERVO_DEG_RANGE_MAX = 12;
//   const int SERVO_DEG_RANGE_MIN = -1 * SERVO_DEG_RANGE_MAX;
// #endif
// #ifdef USE_Servo_360_Feetech360
//   const int MIN_PWM_360 = 700;
//   const int MAX_PWM_360 = 2300;
//   // const int START_DEGREE_VALUE_SERVO_360 = 90;     // 360度サーボの停止位置：仕様では90で停止
//   const int START_DEGREE_VALUE_SERVO_360 = 93;        // サーボ個体差で、90度指定で停止しなかった場合値を変えてみる（試作に使用したsg90-hvの場合95付近で停止だった)
//   const int SERVO_DEG_RANGE_MAX = 6;
//   const int SERVO_DEG_RANGE_MIN = -1 * SERVO_DEG_RANGE_MAX;
// #endif
// #ifdef USE_Servo_360_M5Stack
//   const int MIN_PWM_360 = 500;
//   const int MAX_PWM_360 = 2500;
//   const int START_DEGREE_VALUE_SERVO_360 = 90;         // 360度サーボの停止位置：仕様では90で停止（M5Stack公式は停止のレンジが85～95あたりと広めにとられている様子。手元では個体差なし）
//   const int SERVO_DEG_RANGE_MAX = 12;
//   const int SERVO_DEG_RANGE_MIN = -1 * SERVO_DEG_RANGE_MAX;
// #endif

// bool isTestRunning = false;    // サーボテスト動作のフラグ
// unsigned long prevTime180 = 0, prevTime360 = 0;
// unsigned long interval180 = 0, interval360 = 0;

// int servo180_angle = 0; // 180サーボの角度用変数
// int servo360_speed = 0; // 360サーボの速度用変数

// // ボード種別毎のピン設定など
// #ifdef ARDUINO_M5STACK_Core2
//   // M5Stack Core2用のサーボの設定
//   // Port.A X:G33, Y:G32
//   // Port.C X:G13, Y:G14
//   // スタックチャン基板 X:G19, Y:G27
//   #define SERVO_360_PIN 33
//   #define SERVO_180_PIN 32
// #endif
// #ifdef ARDUINO_M5STACK_FIRE 
//   // M5Stack Fireの場合はPort.A(X:G22, Y:G21)のみです。
//   // I2Cと同時利用は不可
//   #define SERVO_360_PIN 22
//   #define SERVO_180_PIN 21
// #endif
// #ifdef ARDUINO_M5Stack_Core_ESP32 
//   // M5Stack Basic/Gray/Go用の設定
//   // Port.A X:G22, Y:G21
//   // Port.C X:G16, Y:G17
//   // スタックチャン基板 X:G5, Y:G2
//   #define SERVO_360_PIN 22
//   #define SERVO_180_PIN 21
// #endif
// #ifdef ARDUINO_M5STACK_CORES3
//   // M5Stack CoreS3用の設定 ※暫定的にplatformio.iniにARDUINO_M5STACK_CORES3を定義しています。
//   // Port.A X:G1 Y:G2
//   // Port.B X:G8 Y:G9
//   // Port.C X:18 Y:17
//   #define SERVO_360_PIN 18 
//   #define SERVO_180_PIN 17
//   #include <gob_unifiedButton.hpp> // 2023/5/12現在 M5UnifiedにBtnA等がないのでGobさんのライブラリを使用
//   goblib::UnifiedButton unifiedButton;
// #endif

// bool core_port_a = false;         // Core1のPortAを使っているかどうか
// // ================================== End

// // ==================================
// // for Servo Running Mode

// // サーボのタイマーを初期化する関数
// void initializeServoTimers() {
//   unsigned long now = millis();
//   prevTime180 = now;
//   prevTime360 = now;
//   interval180 = random(0, 101);  // ボタンを押した直後はすぐに動作するように初期値として短い時間を設定
//   interval360 = random(0, 101);
// }

// // テストモード開始時にタイマーをリセット
// void startTestMode() {
//   isTestRunning = true;
//   initializeServoTimers();  // タイマー初期化
// }

// // テストモード
// int count_180 = 0;
// int count_360 = 0;
// void servoTestRunningMode(unsigned long currentMillis) {

//   // === 180°サーボの動作 (3秒間隔) ===
//   if (currentMillis - prevTime180 >= interval180) {
//     prevTime180 = currentMillis;
//     interval180 = 3000; // 3秒間隔固定
//     servo180.startEaseTo(START_DEGREE_VALUE_SERVO_180 - (count_180 % 8) * 5);
//     count_180 = (count_180 + 1) % 99;
//   }

//   // === 360°サーボの動作 (5秒間隔) ===
//   if (currentMillis - prevTime360 >= interval360) {
//     prevTime360 = currentMillis;
//     interval360 = 5000; // 5秒間隔固定

//     // 60 ～ 120 度が示す速度（初期位置は90）
//     servo360.startEaseTo(START_DEGREE_VALUE_SERVO_360 + (count_360 % 7) * 10 - 30);
//     count_360 = (count_360 + 1) % 99;
//   }
// }
// // ================================== End

// // ----------------------------------------------
// void setup() {
//   // 設定用の情報を抽出
//   auto cfg = M5.config();
//   M5.begin(cfg);

//   M5.Speaker.setVolume(100);

// #ifdef ARDUINO_M5STACK_FIRE 
//   // M5Stack Fireの場合、Port.Aを使う場合は内部I2CをOffにする必要がある。
//   M5.In_I2C.release();
// #endif
// #ifdef ARDUINO_M5STACK_CORES3 // CORES3のときに有効になります。
//   // 画面上のタッチパネルを3分割してBtnA, B, Cとして使う設定。
//   unifiedButton.begin(&M5.Display, goblib::UnifiedButton::appearance_t::transparent_all);
// #endif

//   // ログ設定
//   M5.Log.setLogLevel(m5::log_target_display, ESP_LOG_NONE);    // M5Unifiedのログ初期化（画面には表示しない。)
//   M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_INFO);     // M5Unifiedのログ初期化（シリアルモニターにESP_LOG_INFOのレベルのみ表示する)
//   M5.Log.setEnableColor(m5::log_target_serial, false);         // M5Unifiedのログ初期化（ログをカラー化しない。）
//   M5_LOGI("Hello World");                                      // logにHello Worldと表示
//   SD.begin(GPIO_NUM_4, SPI, 25000000);                         // SDカードの初期化
//   delay(2000);                                                 // SDカードの初期化を少し待ちます。

//   system_config.loadConfig(SD, "");                            // SDカードから初期設定ファイルを読み込む
//   if (M5.getBoard() == m5::board_t::board_M5Stack) {           // Core1かどうかの判断
//     if (system_config.getServoInfo(AXIS_X)->pin == 22) {       // サーボのGPIOが22であるか確認（本当は21も確認してもいいかもしれないが省略）
//       // M5Stack Coreの場合、Port.Aを使う場合は内部I2CをOffにする必要がある。バッテリー表示は不可。
//       M5_LOGI("I2CRelease");              // ログに出力
//       avatar.setBatteryIcon(false);       // avatarのバッテリーアイコンを表示しないモードに設定
//       M5.In_I2C.release();                // I2Cを使わないように設定(IMUやIP5306との通信をしないようにします。)※設定しないとサーボの動きがおかしくなります。
//       core_port_a = true;                 // Core1でポートAを使用しているフラグをtrueに設定
//     }
//   } else {
//     avatar.setBatteryIcon(true);          // Core2以降の場合は、バッテリーアイコンを表示可能となる。
//   }

//   // servoの初期化
//   M5_LOGI("attach servo");

//   ESP32PWM::allocateTimer(0); // ESP32Servoはタイマーを割り当てる必要がある
//   ESP32PWM::allocateTimer(1);

//   servo180.setPeriodHertz(50);  // サーボ用のPWMを50Hzに設定
//   servo360.setPeriodHertz(50);
  
//   servo180.attach(SERVO_180_PIN, MIN_PWM_180, MAX_PWM_180);
//   servo360.attach(SERVO_360_PIN, MIN_PWM_360, MAX_PWM_360);
  
//   servo180.setEasingType(EASE_QUADRATIC_IN_OUT);       // 動きの最初と終わりがスムーズになる
//   servo360.setEasingType(EASE_LINEAR);       // 一定の速度で動かす場合は EASE_LINEAR に変更

//   setSpeedForAllServos(60);

//   servo180.startEaseTo(START_DEGREE_VALUE_SERVO_180);  // 180°サーボを初期位置にセット
//   servo360.startEaseTo(START_DEGREE_VALUE_SERVO_360);  // 360°サーボを停止位置にセット

//   M5.Power.setExtOutput(!system_config.getUseTakaoBase());       // 設定ファイルのTakaoBaseがtrueの場合は、Groveポートの5V出力をONにする。
//   M5_LOGI("ServoType: %d\n", system_config.getServoType());      // サーボのタイプをログに出力

//   // 顔の色変更
//   // TFT_WHITEなど既定の色が多数用意されている
//   // オリジナルの色は以下のコードで定義可能
//   // uint16_t customColor = 0;
//   // customColor = M5.Lcd.color565(255,140,50);
//   cp = new ColorPalette();
//   cp->set(COLOR_PRIMARY, TFT_WHITE);
//   cp->set(COLOR_BACKGROUND, TFT_BLACK);
//   avatar.setColorPalette(*cp);
//   avatar.init(8);
//   avatar.setBatteryIcon(false);                        // バッテリーアイコンの表示／非表示
//   avatar.setSpeechFont(&fonts::lgfxJapanGothicP_12);  // フォントの指定

//   // last_mouth_millis = millis();    // loop内で使用するのですが、処理を止めずにタイマーを実行するための変数です。一定時間で口を開くのとセリフを切り替えるのに利用します。

//   // // ランダム動作用の変数初期化、個体差を出すためMACアドレスを使用する
//   // uint8_t mac[6];
//   // esp_efuse_mac_get_default(mac);
//   // uint32_t seed = mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24);
//   // randomSeed(seed);
//   // interval180 = random(5000, 10001); // 5秒〜10秒のランダム間隔
//   // interval360 = random(7000, 30001); // 7秒〜30秒のランダム間隔

// }

// // ----------------------------------------------
// char speechText[100];  // フォーマットされた文字列を格納するためのバッファ、Avatarの吹き出しに変数を使用する場合に使用
// void loop() {

// // CoreS3のボタン処理有効化（画面=タッチパネルを[右][中央][左]に3等分した領域がそれぞれBtnA, BtnB, BtnCに対応）
// // 関数内でボタン処理を行う場合は、各関数の中でM5.update();とセットで必ず宣言
// #ifdef ARDUINO_M5STACK_CORES3
//   unifiedButton.update(); // M5.update() よりも前に呼ぶ事
// #endif
//   M5.update();

//   // === ボタンAが押されたらテスト動作モードの開始/停止を切り替え ===
//   if (M5.BtnA.wasPressed()) {
//     M5.Speaker.tone(1500, 200);
//     avatar.setExpression(Expression::Happy);
//     avatar.setSpeechText("テストモード");
//     if (!isTestRunning) {
//       startTestMode();
//     } else {
//       isTestRunning = false;
//       servo180.startEaseTo(START_DEGREE_VALUE_SERVO_180);  // 180°サーボを初期位置に戻す
//       servo360.startEaseTo(START_DEGREE_VALUE_SERVO_360);  // 360°サーボを停止
//       avatar.setExpression(Expression::Neutral);
//       avatar.setSpeechText("");
//     }
//   }

//   // === 強制停止 ===
//   if (M5.BtnB.wasPressed()) {
//     M5.Speaker.tone(2500, 200);
//     avatar.setSpeechText("停止");
//     isTestRunning = false;
//     servo180.startEaseTo(START_DEGREE_VALUE_SERVO_180);  // 180°サーボを初期位置に戻す
//     servo360.startEaseTo(START_DEGREE_VALUE_SERVO_360);  // 360°サーボを停止
//     avatar.setExpression(Expression::Neutral);
//     delay(1500);
//     avatar.setSpeechText("");
// }

//   if (!isTestRunning) return;  // 停止中なら何もしない

//   unsigned long currentMillis = millis();
//   if (isTestRunning) servoTestRunningMode(currentMillis);

// }
