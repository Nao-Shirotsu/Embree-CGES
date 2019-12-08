# Embree3 入門解説 処理フロー概要

## はじめに

[Embree]( https://www.embree.org/index.html )とは、Intelが提供する高速なレイトレーシングを実現するC言語(C99)のライブラリです。
EmbreeはCPUのみで処理を実行し、非常に高いパフォーマンスを発揮してくれます。
この記事では、これからEmbree 3.x系を利用する人へ向け、C++による実装を処理の流れに沿って解説します。

## 想定読者

この記事の想定読者は以下の通りです。関連するトピックの用語を前置き無く使用しますのでご了承ください。  

- Embreeを利用し始めたいが勝手が分からず困っている方
- 基本的なC++の記法が分かる方
- レイトレーシングの基本的な描画フローが分かる方
- Graphics PipelineのVertex Buffer, Index Bufferを知っている方

## 開発環境

執筆時の開発環境と、利用したEmbree3以外のライブラリを挙げます。
また、以後の解説で開発環境(後述)ごとに異なる部分は触れませんのでご了承ください。

- Windows10 Build_18956 insider preview
- Visual Studio 2019
- OpenGL v4.5
- GLFW v3.3.0.1
- GLM v0.9.9.600

※ OpenGLとGLFWはウィンドウへの描画にのみ利用します。   

## Embree APIバージョン

Embreeは、2018年3月に発表された2.x系から3.0へのアップデートの際にAPIが刷新されました。
2.x系と3.x系では処理の流れは概ね同じですが、2.x系のAPIのほとんどが3.x系で利用できません。
この記事では、**Embree 3.x系に沿って**解説します。(以降Embree3と表記)   
筆者も大いに参考にさせていただいたEmbree 2.x系の解説記事は[こちら](https://qiita.com/denirou/items/1933dbb1dceb59e3aa3c)。


## 描画モデル

![tetra_bottom](C:\Users\albus\Pictures\gomi\tetra_bottom.png)

今回はこのモデル(頂点数6, ポリゴン数8)を利用して解説します。
以下にC++コードで便宜的に示します。

```
vertex[0] = { -0.5f,  0.0f,  0.0f };
vertex[1] = { -0.0f,  1.0f,  0.0f };
vertex[2] = {  0.5f,  0.0f,  0.0f };
vertex[3] = {  0.0f, -1.0f,  0.0f };
vertex[4] = {  0.0f,  0.0f, -0.5f };
vertex[5] = {  0.0f,  0.0f,  0.5f };
index[0] = { 1, 2, 5 };
index[1] = { 2, 3, 5 };
index[2] = { 3, 4, 5 };
index[3] = { 4, 1, 5 };
index[4] = { 2, 1, 6 };
index[5] = { 3, 2, 6 };
index[6] = { 4, 3, 6 };
index[7] = { 1, 4, 6 };
```




## 環境構築

[公式ページ]( https://www.embree.org/downloads.html )から依存ファイルをダウンロードし、お好きなディレクトリへインストールしてください。
ヘッダファイル, リンクファイルの設定もお忘れなく。

ちなみに、旧バージョンは[公式開発リポジトリのReleases](https://github.com/embree/embree/releases)から入手できます。

## Embree3のオブジェクト

Embree3にはDevice, Scene, Geometry, Buffer, BVH というオブジェクトがあります。
(これらはEmbree3で特殊な意味を持つので、この記事上では曖昧さ回避のため便宜的に「Embree3オブジェクト」と表記します)
これらに対応する型のインスタンスは通常`rtcNewXXX`(XXXにDevice等が入る)という関数で生成し、その返り値のハンドルをユーザが利用します。
また、これらのオブジェクトは参照カウント方式で管理できます。`rtcNewXXX`で生成した直後はカウントが1で、`rtcRetainXXX`関数でインクリメント、`rtcReleaseXX`関数でデクリメントします。リリース時にカウントが0ならばインスタンスが解放されます。
(この一連の操作を「ユーザ側で管理」と表現します)

## 処理フロー概要

![embree_qiita_api](C:\Users\albus\Pictures\gomi\embree_qiita_api.png)

Embree3ではこのような流れでプログラムを記述していきます。
下記の各解説では、**利用する関数のシグネチャ -> 呼び出し側のコード** という順で表記します。
また、「Embree3のKernel」という単語を「APIコールによってEmbree3の内部で処理をする部分」という意味で用います。

## デバイス生成

Embree3オブジェクト、Deviceを生成します。
返り値はハンドルとして受け取り、ユーザ側で管理します。

```C++
// declared in embree3/rtcore_device.h
struct RTCDevice;
RTCDevice rtcNewDevice(const char* config);
```

```C++
RTCDevice device = rtcNewDevice(nullptr);
```

このデバイスは、他のEmbree3オブジェクトの生成に利用します。
また、引数に特定の文字列を渡すことでconfiguration)ができます。

#### `rtcNewDevice`のconfiguration項目

空の文字列、`NULL`、`nullptr`を渡すとデフォルト設定になります。

それ以外
以下は設定項目リストと概説です。

- `threads=[int]` 
  ビルドスレッドの数？
  
- `user_threads=[int]`
  シーンコミット`rtcJoinCommitScene`に使うスレッドの数？
  
- `set_affinity=[0/1]`
  enable : ビルドスレッドがaffinitizeされる？(Xeon Phiでのみ可っぽい)
  
- `start_threads=[0/1]`
  ビルドスレッドが先行して実行される？ スレッド生成時間を除いてベンチマークする時に便利。
  
- `isa=[sse2,sse4.2,avx,avx2,avx512knl,avx512skx]`
  利用するCPU命令セットアーキテクチャを選択します。何も指定しなければ自動で選択されます。
  
- `max_isa=[sse2,sse4.2,avx,avx2,avx512knl,avx512skx]`
  利用する命令セットアーキテクチャの自動選択のうち最高のものを指定します。指定したアーキテクチャより良いアーキテクチャは自動選択されなくなります。
  
- `hugepages=[0/1]`
  huge pagesを有効化/無効化します。Linux環境ではデフォルトで有効、MacとWindows環境ではデフォルトで無効です。
  
- `enable_selockmemoryprivilege=[0/1]`
  有効化すると、?
  
- `ignore_config_files=[0/1]`
  configuration filesを無視する?
  
- `verbose=[0,1,2,3]`
  Embree3による情報出力の冗長性を操作します。デフォルトでは何も出力しない0で、値を大きくするとより多くの情報を出力するようになります。

- `frequency_level=[simd128,simd256,simd512]`

  

## シーン初期化

「3D空間上のオブジェクト」を保持するシーンを生成します。
返り値はハンドル(`RTCScene`型)として受け取り、ユーザ側で管理します。

```C++
// declared in embree3/rtcore_scene.h
struct RTCScene;
RTCScene rtcNewScene(RTCDevice device);
```

```C++
RTCScene scene = rtcNewScene(device);
```

後に `scene` に「3D空間上のオブジェクト」を1つ以上アタッチし、レイトレースで利用します。

## ジオメトリ初期化

#### ジオメトリ生成

「3D空間上のオブジェクト」ことジオメトリを生成します。
返り値のハンドル(`RTCGeometry`型)を一時的に受け取りますが、この後の「シーンへのジオメトリ登録」の直後に解放し、ユーザ管理から切り離すことが できます。   

```C++
// declared in embree3/rtcore_geometry.h
struct RTCGeometry;
RTCGeometry rtcNewGeometry(RTCDevice device, RTCGeometryType type);
```

```C++
RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
```

`rtcNewGeometry`の第2引数にはEmbree3が定義しているenum値`RTCGeometryType`を渡します。
このenum値によって、この「3D空間上のオブジェクト」がどういう形式のバッファとして確保されるかをEmbree3のKernel側に伝えます。      
例えば`RTC_GEOMETRY_TYPE_TRIANGLE`を渡せば、ポリゴンメッシュ形式(Vertex Buffer & Index Buffer)でメモリ上に確保されることを指定できます。   

#### バッファの実体確保とジオメトリへのバインド

次に、「3D空間上のオブジェクト」の実体を確保してジオメトリにバインドします。   
実体の動的確保をEmbree3の関数にやってもらう方法と、ユーザ自身で確保してEmbree3のKernel側に伝える方法があります。     
順に記述します。

```C++
// declared in embree3/rtcore_geometry.h
void* rtcSetNewGeometryBuffer(
  RTCGeometry geometry,    // 上で生成したジオメトリ
  enum RTCBufferType type, // バッファ形式
  unsigned int slot,       // 
  enum RTCFormat format,   // 要素1つが持つ変数の型と数の指定
  size_t byteStride,       // 要素1つ分のサイズ
  size_t itemCount         // 要素数
);
```

```C++
struct Vertex {
  float x, y, z;
};
struct PolygonIndex{
  unsigned int v0, v1, v2;
}
// ~中略~
constexpr size_t numVertex = 6;
Vertex* geometryVertices = reinterpret_cast<Vertex*>(rtcSetNewGeometryBuffer(geometryHandle,
                                                                               RTC_BUFFER_TYPE_VERTEX,
                                                                               0,
                                                                               RTC_FORMAT_FLOAT3,
                                                                               sizeof(Vertex),
                                                                               numVertex));

Index* geometryIndices = reinterpret_cast<PolygonIndex*>(rtcSetNewGeometryBuffer(geometryHandle,
                                                                          RTC_BUFFER_TYPE_INDEX,
                                                                          0,
                                                                          RTC_FORMAT_UINT3,
                                                                          sizeof(PolygonIndex),
                                                                          numPolygons));
```

`rtcSetNewGeometryBuffer`を利用すると、Embree3が引数からサイズを計算してバッファ領域の動的確保をしてくれます。   
上記の例では、返り値の`void *`を`geometryVertices`で受け取り、今後配列として利用します。

```C++
// declared in embree3/rtcore_geometry.h
void rtcSetSharedGeometryBuffer(
  RTCGeometry geometry,
  enum RTCBufferType type,
  unsigned int slot,
  enum RTCFormat format,
  const void* ptr,
  size_t byteOffset,
  size_t byteStride,
  size_t itemCount
);
```

```C++
struct Vertex {
  float x, y, z;
};
struct PolygonIndex{
  unsigned int v0, v1, v2;
}
// ~中略~
constexpr size_t numVertex = 6;
constexpr int numPolygons = 8;
std::array<Vertex, numVertex> geometryVertices;
std::array<PolygonIndex, numPolygons> geometryPolygons;

rtcSetSharedGeometryBuffer(geometryHandle,
                           RTC_BUFFER_TYPE_VERTEX,
                           0,
                           RTC_FORMAT_FLOAT3,
                           &geometryVertices,
                           0,
                           sizeof(Vertex),
                           geometryVertices.size());
rtcSetSharedGeometryBuffer(geometryHandle,
                           RTC_BUFFER_TYPE_INDEX,
                           0,
                           RTC_FORMAT_UINT3,
                           &geometryPolygons,
                           0,
                           sizeof(PolygonIndex),
                           geometryPolygons.size());
```

`rtcSetSharedGeometryBuffer`を利用すると、既にユーザ側で確保したバッファ領域をジオメトリの実体として利用できます。   
上記の例では、`std::array<Vertex, numVertex>`として定義した`geometryVertices`のアドレスを引数に渡します。    

#### バッファへのデータ入力

次に、上のいずれかの方法で定義したジオメトリに頂点データを入力します。    
今回は手入力としますが、この処理を3Dモデルデータ読込処理に差し替えてしまえば、インポートが実現できます。     
`rtcSetSharedGeometryBuffer`を利用してバッファ領域確保を行なう場合は、`geometryVertices`の定義時に以下のデータ入力処理をしておいても問題ありません。   

```C++
// Embree3の関数は使用しません。
```

```C++
geometryVertices[0] = {-0.5f,  0.0f,  0.0f};
geometryVertices[1] = {-0.0f,  1.0f,  0.0f};
geometryVertices[2] = { 0.5f,  0.0f,  0.0f};
geometryVertices[3] = { 0.0f, -1.0f,  0.0f};
geometryVertices[4] = { 0.0f,  0.0f, -0.5f};
geometryVertices[5] = { 0.0f,  0.0f,  0.5f};
geometryPolygons[0] = { 1, 2, 5 };
geometryPolygons[1] = { 2, 3, 5 };
geometryPolygons[2] = { 3, 4, 5 };
geometryPolygons[3] = { 4, 1, 5 };
geometryPolygons[4] = { 2, 1, 6 };
geometryPolygons[5] = { 3, 2, 6 };
geometryPolygons[6] = { 4, 3, 6 };
geometryPolygons[7] = { 1, 4, 6 };
```

   


#### ジオメトリのコミット **変更で結果変わらん云々、ほんまか？

ジオメトリのバッファのデータ入力まで終えた後、このジオメトリの状態をEmbree3のKernel側へ伝えます。   

```C++
// declared in embree3/rtcore_geometry.h
void rtcCommitGeometry(RTCGeometry geometry);
```

```c++
rtcCommitGeometry(geometry);
```

今後`geometryVertices`の要素に少しでも変更があった場合、この関数を呼び出さなければその変更がEmbree3のKernel側に伝わらず、後のレイトレースの結果も変わりません。   
例えば、リアルタイムレンダリングでフレームごとににオブジェクトを移動させたいとき、毎フレームの更新処理でこの関数を呼び出す必要があります。   

##  シーンへのジオメトリ登録

前ステップで初期化したジオメトリをシーンに登録します。   
1つのシーンに複数のジオメトリを登録することももちろん可能です。

```C++
// declared in embree3/rtcore_scene.h
unsigned int rtcAttachGeometry(
  RTCScene scene,
  RTCGeometry geometry
);
```

```C++
rtcAttachGeometry(geometry);
```


登録の後、ジオメトリを解放することができます。   

```C++
// declared in embree3/rtcore_geometry.h
void rtcReleaseGeometry(RTCGeometry geometry);
```

```C++
rtcReleaseGeometry(geometry);
```

この関数呼び出しは、`rtcAttachGeometry`によってシーンが`geometry`への参照を獲得したため、**ユーザが管理をする必要が無くなったから解放しよう**ということです。   
**必ずしもこのタイミングでは**ジオメトリ解放を行なう必要はありません。    

##  シーン構築

シーンの状態をEmbree3のKernel側へ伝えます。

```C++
// declared in embree3/rtcore_scene.h
void rtcCommitScene(RTCScene scene);
```

```C++
rtcCommitScene(scene);
```

この関数が呼び出されると、Embree3は内部的に[BVH]( https://en.wikipedia.org/wiki/Bounding_volume_hierarchy )の構築を行ないます。   
ここまでで描画するシーンのセットアップがすべて完了となります。   

## レイトレース

#### 1本のレイを飛ばす

```C++
// declared in embree3/rtcore_common.h
struct RTCIntersectContext;
void rtcInitIntersectContext(RTCIntersectContext* context);

// declared in embree3/rtcore_ray.h
struct RTCRay{
  float org_x;        // x coordinate of ray origin
  float org_y;        // y coordinate of ray origin
  float org_z;        // z coordinate of ray origin
  float tnear;        // start of ray segment

  float dir_x;        // x coordinate of ray direction
  float dir_y;        // y coordinate of ray direction
  float dir_z;        // z coordinate of ray direction
  float time;         // time of this ray for motion blur

  float tfar;         // end of ray segment (set to hit distance)
  unsigned int mask;  // ray mask
  unsigned int id;    // ray ID
  unsigned int flags; // ray flags
};
struct RTCHit
{
  float Ng_x;          // x coordinate of geometry normal
  float Ng_y;          // y coordinate of geometry normal
  float Ng_z;          // z coordinate of geometry normal

  float u;             // barycentric u coordinate of hit
  float v;             // barycentric v coordinate of hit

  unsigned int primID; // primitive ID
  unsigned int geomID; // geometry ID
  unsigned int instID[RTC_MAX_INSTANCE_LEVEL_COUNT]; // instance ID
};
struct RTCRayHit{
  struct RTCRay ray;
  struct RTCHit hit;
};
void rtcIntersect1(
    RTCScene scene, 
    RTCIntersectContext* context,
    RTCRayHit* rayhit
);
```

```C++
constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();

RTCIntersectContext context;
rtcInitIntersectContext(&context);

RTCRayHit rayhit; // (0, 0, -10)から(0, 0, 10)方向へのレイ
rayhit.ray.org_x = 0.0f;
rayhit.ray.org_y = 0.0f;
rayhit.ray.org_z = -10.0f; 
rayhit.ray.dir_x = 0.0f;
rayhit.ray.dir_y = 0.0f;
rayhit.ray.dir_z = 10.0f;
rayhit.ray.tnear = 0; // (0, 0, -10)から距離0の位置からキャスト
rayhit.ray.tfar = FLOAT_INFINITY; // レイを無限遠まで飛ばす
rayhit.ray.mask = 0;
rayhit.ray.flags = 0;
rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

rtcIntersect1(sceneHandle, &context, &rayhit);

if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
    // シーン上の何かしらと交差した
}
else{
    // 何にも交差しなかった
}
```

`RTCRayHit`のインスタンスを生成し、`rtcIntersect1`によって交差判定を行います。   
すると、`rayhit.hit.geomID`に、交差した最も近いジオメトリのIDが保存されます。    
`RTCRayHit::dir_x`ら方向ベクトル成分は正規化されている必要はありません。   

#### スクリーンにレイを飛ばす

今回は簡単のため、カメラ位置などは考慮せず、平行投影と同様な交差判定をします。   
![screen](C:\Users\albus\Pictures\gomi\screen.png) ![screenxy](C:\Users\albus\Pictures\gomi\screenxy.png)

#### 

```C++
// 1本の場合と同様
```

```C++
constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();

RTCIntersectContext context;
rtcInitIntersectContext(&context);

RTCRayHit rayhit; // (0, 0, -10)から(0, 0, 10)方向へのレイ
rayhit.ray.org_x = 0.0f;
rayhit.ray.org_y = 0.0f;
rayhit.ray.org_z = -10.0f; 
rayhit.ray.dir_x = 0.0f;
rayhit.ray.dir_y = 0.0f;
rayhit.ray.dir_z = 10.0f;
rayhit.ray.tnear = 0;
rayhit.ray.tfar = FLOAT_INFINITY;
rayhit.ray.mask = 0;
rayhit.ray.flags = 0;
rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

rtcIntersect1(sceneHandle, &context, &rayhit);

if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
    // シーン上の何かしらと交差した
}
else{
    // 何にも交差しなかった
}
```



## 終了処理

全てのオブジェクトを解放します。

```C++
// declared in embree3/rtcore_scene.h
void rtcReleaseScene(RTCScene scene);

// declared in rtcore_device.h
void rtcReleaseDevice(RTCDevice device);
```

```C++
rtcReleaseScene(sceneHandle);
rtcReleaseDevice(deviceHandle);
// その他必要な解放処理
```



## ソースコード(全体)

```C++
rtcCommitGeometry(geometry);
```



## おわりに

お疲れさまでした。  
次回の解説ではシェーダプログラムを利用しないPhong shading実装まで行います。  
もしこの記事内に間違いや不適切な記述を見つけた場合は、コメントでお教えください。   



## 参考記事

EmbreeによるCPUレンダリング(その１) 交差判定 : https://qiita.com/denirou/items/1933dbb1dceb59e3aa3c
Embree公式ページ(Overview) : https://www.embree.org/index.html
Embree公式ページ(API reference) : https://www.embree.org/api.html
Embree公式リポジトリ : https://github.com/embree/embree
Wikipedia BVH :  https://en.wikipedia.org/wiki/Bounding_volume_hierarchy 

