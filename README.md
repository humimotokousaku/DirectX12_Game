[![Debug](https://github.com/humimotokousaku/DirectX12_Game/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/humimotokousaku/DirectX12_Game/actions/workflows/DebugBuild.yml)
[![Release](https://github.com/humimotokousaku/DirectX12_Game/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/humimotokousaku/DirectX12_Game/actions/workflows/ReleaseBuild.yml)
# 就職作品について
## 3Dルート固定レールシューティング
スターフォックス64やEx-zodiacのような動かしているだけで気持ちいい自機の操作性や、障害物をぎりぎりでよける緊張感を目指して製作に取り組んでいます

## ゲーム概要
迫りくる敵や障害物をひたすらよけてゴールを目指する3Dルート固定レールシューティングです。  
ゲームスピードが速いので反射神経が求められるゲームです。  
![](ReadMe_resource/GameScene.gif)

## 自機の挙動  
操作しているだけで気持ち良いと思えるように自機の動きを大げさにし、カメラの動きも激しくなるような演出を実装しました。  

 - ### ブースト機能
   コントローラーのXボタンを押している間加速します。加速中はラジアルブラーをかけ、カメラのFovを上げて通り過ぎるものがより速く見えるようにしました。
   ![](ReadMe_resource/GameScene_Boost.gif)
  
 - ### 回避機能
   コントローラーAボタンを押すと移動方向に向かって素早く移動します。回避時は自機から残像が出るようにしました。
   レスポンスを向上させるため、回避の硬直は少なくして、何度でも押しやすいようにしました。
   ![](ReadMe_resource/GameScene_Evasion.gif)
  
## Blenderを利用したレベルエディタ
Blenderのアドオン開発を行い以下の設定を行えるようにしました。

### 敵の出現情報  
"敵の出現場所を生成"タブをクリックするとトーラスモデルとNURBSパスが生成されます。トーラスモデルは敵の向きを表し、NURBSパスは敵の移動ルートです。
![](ReadMe_resource/Blender_Editor_00.png)

 - #### 種類  
 ![](ReadMe_resource/Blender_Editor_01.png)
 - #### 出現タイミング
 レールカメラがどのくらい進んだら敵を発生させるかを設定することができます  
 ![](ReadMe_resource/Blender_Editor_02.png)
 - #### 移動ルートの編集
 ![](ReadMe_resource/Blender_Editor_03.png)

### 障害物の配置  
 - #### モデル配置
 ファイルの名前を入力後にEngine/resources/Levelフォルダー内に該当のモデルを入れることでモデルの配置することができます。
 ![](ReadMe_resource/Blender_Editor_Object.png)

 - #### 当たり判定の設定  
 配置したモデルに当たり判定を付与することができ、水色のワイヤーフレームが出現します。  
 タイプ:当たり判定の種類を選択できます
 サイズ:当たり判定の大きさを変更できます    
 ![](ReadMe_resource/Blender_Editor_Collider.png)

### レールカメラのルートを編集
"レールカメラの移動ルートの生成"タブをクリックするとNURBSパスが生成されます。  
敵の発生タイミングが調整しやすくなるするために各制御点の位置がカーブ全体の何パーセントにいるのかを表示しています。  
![](ReadMe_resource/Blender_Editor_RailCamera_01.png)

## 今後の展望
敵挙動の作成やタイトルシーン、クリアシーン等の画面作りを行いゲーム全体の完成度を上げていきたいです

## 使用した外部ライブラリ
- assimp
- DirectXTex
- nlohmann
- ImGui
