## `RTCRayHit`の宣言位置に気をつける

```C++
RTCRayHit rayhit;
    rayhit.ray.org_x = 0;
    rayhit.ray.org_z = -5;
    rayhit.ray.dir_x = 0;
    rayhit.ray.dir_y = 0;
    rayhit.ray.dir_z = 1;
    rayhit.ray.tnear = 0;
    rayhit.ray.tfar = 100000000;
    rayhit.ray.mask = 0;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
for (int i = 0; i < 30; ++i) {
    rayhit.ray.org_y = -1.6 + 0.1 * i;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rtcIntersect1(scene, &context, &rayhit);
    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
      std::cout << "Intersected";
    }
    else {
      std::cout << "Not intersected";
    }
    std::cout << " at y=" << rayhit.ray.org_y << std::endl;
  }
```

こんな感じでループ中同じ`rayhit`を値だけ変えてやるとダメ。   
なんか知らんが、前のループの判定を持ったまま引き継がれるので正確に交差判定ができない。

```C++
for (int i = 0; i < 30; ++i) {
    RTCRayHit rayhit;
    rayhit.ray.org_x = 0;
    rayhit.ray.org_y = -1.6 + 0.1 * i;
    rayhit.ray.org_z = -5;
    rayhit.ray.dir_x = 0;
    rayhit.ray.dir_y = 0;
    rayhit.ray.dir_z = 1;
    rayhit.ray.tnear = 0;
    rayhit.ray.tfar = 100000000;
    rayhit.ray.mask = 0;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
    rtcIntersect1(scene, &context, &rayhit);
    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
      std::cout << "Intersected";
    }
    else {
      std::cout << "Not intersected";
    }
    std::cout << " at y=" << rayhit.ray.org_y << std::endl;
  }
```



## `rtcNewGeometry`に渡すenum値に気をつける

Embree3ではジオメトリプリミティブが何種類か用意されていて、オブジェクトのインスタンスを初期状態で生成する`rtcNewGeometry`を呼び出すときに引数にenum値(`RTC_GEOMETRY_TYPE_TRIANGLE`など)を渡してインスタンス化する。   
その後VertexBufferなどの動的確保領域のアドレスとサイズを`rtcSetSharedGeometryBuffer`でEmbreeRTKernel側に伝えるが、その時前に渡したenum値によってはリジェクトされてエラーになることがある。   

```C++
auto geometry = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE);
rtcSetSharedGeometryBuffer(
      m_geometry,
      RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
      0,
      RTC_FORMAT_FLOAT8,
      m_verBuf.data(),
      0,
      sizeof(Vertex3f),
      m_verBuf.size());

  rtcSetSharedGeometryBuffer(
      m_geometry,
      RTC_BUFFER_TYPE_INDEX,
      0,
      RTC_FORMAT_UINT4,
      m_idxBuf.data(),
      0,
      sizeof(PolygonIndices),
      m_idxBuf.size());
```

こんな感じにしたとき、`RTC_GEOMETRY_TYPE_TRIANGLE`だと`rtcSetSharedGeometryBuffer`はStrideが`float`3つのVertexBuffer, `uint32_t`3つのIndexBuffer以外は登録してくれない。   
結果`rtcSetSharedGeometryBuffer`がデバイスエラーを登録してくれちゃって書いてる側の胃が死ぬ。



## `rtcInterpolate`のための頂点属性

Embree3にはバッファに `slot` というものが用意されていて、このスロットを `1` 以上にすることで頂点とインデックス以外のバッファを一つのジオメトリにアタッチできる。   
スロットを `1` 以上にするには、`rtcSetGeometryVertexAttributeCount(RTCGeometry, size_t numSlot)`の引数で指定する。   
多分`rtcSetSharedGeometryBuffer`の引数に`bufferSlot`みたいな名前があったのでたぶんここでも指定できる。(要検証)

#### `RTC_GEOMETRY_TYPE_TRIANGLE`の場合

ジオメトリのこのタイプ(Triangle meshes)にした場合はこんな感じ。

```C++
struct Vertex3f {
  float x, y, z;
};

struct VertexNormal3f {
  float nx, ny, nz;
};

struct PolygonIndices {
  unsigned int v0, v1, v2;
};

// ~~~中略~~~

auto m_rtcGeometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
std::vector<Vertex3f> m_verBuf;
std::vector<PolygonIndices> m_idxBuf;
std::vector<VertexNormal3f> m_verNormalBuf;

// ~~~中略~~~

rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT3,
      m_verBuf.data(),
      0,
      sizeof(Vertex3f),
      m_verBuf.size());

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_INDEX,
      0,
      RTC_FORMAT_UINT3,
      m_idxBuf.data(),
      0,
      sizeof(PolygonIndices),
      m_idxBuf.size());

	

  rtcSetGeometryVertexAttributeCount(m_rtcGeometry, 1); 
  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
      0,
      RTC_FORMAT_FLOAT3,
      m_verNormalBuf.data(),
      0,
      sizeof(VertexNormal3f),
      m_verNormalBuf.size());

  rtcCommitGeometry(m_rtcGeometry);
```

