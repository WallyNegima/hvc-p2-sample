# 見守りプロジェクト カメラモジュール HVC-P2
最終更新日:20170922

- 使い方
  - 顔を認識したい
    - 実行形式ファイル:exe-camera
  - 顔を登録したい
    - 実行形式ファイル:faceRegister
    - 実行時にid，dataid，人の名前を入力すること
      - 例: faceRegister 0 0 ほげたろう
      - 既にIDに別の名前が登録されていれば上書きする
    - code直下にalbum.txtというアルバムデータが作成される
  - アルバムデータをカメラに読み込む
    - 実行形式ファイル:libTester
    - コマンドがいくつか用意されている
      - 1:アルバムデータをホストからカメラへ
      - 2:カメラ内のアルバムデータをROMへ
      - の順序で行うことでalbum.txtのアルバムデータをカメラに保持できる

- シリアルルール
	- 99-serial.rulesを/etc/udev/rules.d/ 直下に置く
	- sudo /etc/init.d/uedv reload を実行
	- hvcp2 というモジュールが使用可能になる

- プログラム
  - main.cpp
    - 顔を認識した結果を返す
    - 2秒おきに認識
    - 顔を認識後7秒待機(喋るので)
    - make コマンドでコンパイル
    - exe-camera で実行
  - faceRegister.cpp
    - 顔を登録するプログラム
    - カメラのROMに登録後，ホストのalbum.txtにデータを格納
    - make faceRegister でコンパイル
    - faceRegister userid userdata で実行(userid,userdataには登録したい数字)
  - hvcp2-lib.h
    - コマンドを色々集めたライブラリ
  - test.cpp
    - ライブラリをテストするためのプログラム
    - make test でコンパイル
    - libTester で実行
