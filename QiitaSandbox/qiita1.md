# Embree3 入門解説 (API概要と交差判定)

## はじめに

[Embree]( https://www.embree.org/index.html )とは、Intelが提供する高速なレイトレーシングを実現するライブラリです。   
この記事では、これからEmbree 3.x系を利用する人へ向け、APIの利用を処理の流れに沿って解説します。     
     

## この記事の対象読者

この記事の想定読者は以下の通りです。これらに関連する用語を前置き無く使用しますのでご了承ください。  

- 基本的なC++の記法が分かる方
- レイトレーシングによる基本的な描画フローを理解している方   
- Graphics APIにおける基本的なバッファの概念を理解している方    
  

## Embree APIバージョン

Embreeは、2018年3月に発表された2.x系から3.0へのアップデートの際にAPIが刷新されました。  
2.x系と3.x系では処理の流れは概ね同じですが、2.x系のAPIのほとんどが3.x系で利用できません。    
この記事では、**Embree 3.x系の内容を取り扱います。**(以降Embree3と表記)     

筆者も大いに参考にさせていただいたEmbree 2.x系の解説記事は[こちら](https://qiita.com/denirou/items/1933dbb1dceb59e3aa3c)。     



## 開発環境

執筆時の開発環境と利用したEmbree3以外のライブラリを挙げます。      
また、開発環境(後述)ごとに異なる部分の解説も行いません。

- Windows10 Build_18950, insider preview
- Visual Studio 2019
- OpenGL v4.5
- GLFW v3.3.0.1
- GLM v0.9.9.600    

※ OpenGLとGLFWはウィンドウへの描画にのみ利用します。    



## 環境構築

[公式ページ]( https://www.embree.org/downloads.html )から依存ファイルをダウンロードし、解凍して適切なディレクトリへ配置します。  
開発環境ごとにヘッダファイル, リンクファイルに関する設定を行なってください。   

## 処理フロー概要

<img src="C:\Users\albus\Pictures\gomi\embree_qiita_api.png" alt="embree_qiita_api" />

Embree3ではこのような流れでプログラムを記述していきます。   
順に解説します。     

## デバイス生成

Embree3の制御する生成します。
Graphics APIでおなじみの方もいるかと思います。   





## おわりに

お疲れさまでした。  
次回の解説ではシェーダプログラムを利用しないPhong shading実装まで行います。  
もしこの記事内に間違いや不適切な記述を見つけた場合は、コメントでお教えください。   



## 参考記事

EmbreeによるCPUレンダリング(その１) 交差判定 : https://qiita.com/denirou/items/1933dbb1dceb59e3aa3c
Embree公式ページ(Overview) : https://www.embree.org/index.html
Embree公式ページ(API reference) : https://www.embree.org/api.html
Embree公式リポジトリ : https://github.com/embree/embree

