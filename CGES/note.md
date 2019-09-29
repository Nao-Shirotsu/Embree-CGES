### `RTCRayHit`の宣言位置に気をつける

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

