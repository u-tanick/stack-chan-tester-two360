#include <Arduino.h>
#include <M5Unified.h>
#include <Servo.h>

// ====== ピン設定 ======
#define SERVO_LEFT_PIN 33
#define SERVO_RIGHT_PIN 32

// ====== パラメータ ======
int STOP_VAL = 90;       // サーボ停止位置
int SPEED = 30;          // 速度
int MOVE_TIME = 2000;    // 動作時間 [ms]
int PAUSE_TIME = 1000;   // 停止時間 [ms]

// ====== グローバル変数 ======
Servo servoLeft;  
Servo servoRight;

bool testRunning = false;      // テストモード実行中フラグ
bool stopRequested = false;    // 強制停止フラグ

// ====== 関数宣言 ======
void runTestSequence();
void forward();
void backward();
void turnRight();
void turnLeft();
void stopMotors();
void delayWithStopCheck(int duration);

// =================================
void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);

    servoLeft.attach(SERVO_LEFT_PIN, 500, 2500);
    servoRight.attach(SERVO_RIGHT_PIN, 500, 2500);

    stopMotors();
    M5.Display.println("Press BtnA to start test");
}

// =================================
void loop() {
    M5.update();

    // Bボタンで強制停止
    if (M5.BtnB.wasPressed() && testRunning) {
        stopRequested = true;
        M5.Display.println("Force Stop");
    }

    // Aボタンでテスト開始
    if (M5.BtnA.wasPressed() && !testRunning) {
        testRunning = true;
        stopRequested = false;
        runTestSequence();
        testRunning = false;
        stopMotors();
        M5.Display.println("Test finished");
    }
}

// =================================
// テストシーケンス（割り込み対応）
void runTestSequence() {
    if (stopRequested) return;
    forward();    delayWithStopCheck(MOVE_TIME);

    if (stopRequested) return;
    stopMotors(); delayWithStopCheck(PAUSE_TIME);

    if (stopRequested) return;
    backward();   delayWithStopCheck(MOVE_TIME);

    if (stopRequested) return;
    stopMotors(); delayWithStopCheck(PAUSE_TIME);

    if (stopRequested) return;
    turnRight();  delayWithStopCheck(MOVE_TIME);

    if (stopRequested) return;
    stopMotors(); delayWithStopCheck(PAUSE_TIME);

    if (stopRequested) return;
    turnLeft();   delayWithStopCheck(MOVE_TIME);

    if (stopRequested) return;
    stopMotors(); delayWithStopCheck(PAUSE_TIME);
}

// =================================
// モーター制御関数
void stopMotors() {
    servoLeft.write(STOP_VAL);
    servoRight.write(STOP_VAL);
    M5.Display.println("Stop");
}

void forward() {
    servoLeft.write(STOP_VAL + SPEED);
    servoRight.write(STOP_VAL - SPEED);
    M5.Display.println("Forward");
}

void backward() {
    servoLeft.write(STOP_VAL - SPEED);
    servoRight.write(STOP_VAL + SPEED);
    M5.Display.println("Backward");
}

void turnRight() {
    servoLeft.write(STOP_VAL + SPEED);
    servoRight.write(STOP_VAL + SPEED);
    M5.Display.println("Turn Right");
}

void turnLeft() {
    servoLeft.write(STOP_VAL - SPEED);
    servoRight.write(STOP_VAL - SPEED);
    M5.Display.println("Turn Left");
}

// =================================
// 割り込み対応delay
void delayWithStopCheck(int duration) {
    int elapsed = 0;
    int step = 50; // 50msごとにボタンチェック
    while (elapsed < duration) {
        if (stopRequested) break;
        delay(step);
        elapsed += step;
        M5.update(); // ボタン状態更新
    }
}
