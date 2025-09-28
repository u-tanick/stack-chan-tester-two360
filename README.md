# stack-chan-tester-two360

## 概要

M5Stackに顔を表示させるライブラリ [M5Stack-Avatar](https://github.com/stack-chan/m5stack-avatar) を中心に開発されている手乗りロボット `ｽﾀｯｸﾁｬﾝ` での利用を想定し、
サーボモーターを従来の `180度回転を2個` から、`360度回転の2個` を想定したテストプログラムです。

2個のサーボで動作する自動車やロボットに対して、前後や左右旋回の動作検証に使用できます。

従来のスタックチャンのテストプログラムは、[stack-chan-tester](https://github.com/mongonta0716/stack-chan-tester) として @mongonta0716 さんにより公開されています。

本プログラムは、`stack-chan-tester` を素に、改変を行ったものですが、コード理解の参考になるようライブラリのincludeや変数の宣言を整理しなおしたものとなっています。

## 使用ライブラリ

詳細は `platformio.ini` を参照してください。

- meganetaaan/M5Stack-Avatar@0.10.0
- mongonta0716/stackchan-arduino
- gob/gob_unifiedButton @ ^0.1.5
- tobozo/M5Stack-SD-Updater

- その他
  - サーボモーターの制御は `ServoEasing` を使用しています。

## 想定サーボモーター

`M5Stack Servo Kit 360` での動作確認を行っています。

- 360度回転サーボモーターなどでの使用を想定しています
  - [M5Stack Servo Kit 360](https://www.switch-science.com/products/6479)
  - [TowerPro SG90-HV](https://akizukidenshi.com/catalog/g/g114382/)
  - [FEETECH FS90R](https://www.switch-science.com/products/7113)

使用するサーボモーターを変更する場合は、使用するサーボの仕様に合わせてpwmの値を調整してください。

``` cpp
// L:20 付近
#define SERVO_MIN 500
#define SERVO_MAX 2500
#define SERVO_STOP 90        // 停止位置
```

## 想定M5Stack製品

詳細は `platformio.ini` を参照してください。デフォルトでは `m5stack-core2` となっています。

- m5stack-core-esp32
- m5stack-grey
- m5stack-fire
- m5stack-core2
- m5stack-cores3

## 提供機能

- Aボタン
  - テスト駆動モード ON/OFF
  - 次の順序で2個のサーボを動作させるモードです
    - 前進 -> 停止 -> 後退 -> 停止 -> 右旋回 -> 停止 -> 左旋回 -> 停止
- Bボタン
  - 強制停止
  - 強制的に停止します。
