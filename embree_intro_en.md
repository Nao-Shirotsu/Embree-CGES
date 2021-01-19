# Embree Raytracing

In this section, we present how to develop raytracing method-switchable renderer.
This part is based on [Shirotsu Qiita] written in japanese by the author.

## 1. About Embree

Embree is a C99 library that enables fast raytracing on CPU. It is optimized for Intel processors and supports other CPUs that can invoke SIMD intrinsics: SSE, AVX, AVX2, and AVX-512.

One can implement raytracing renderer by calling Embree APIs. The kernel of Embree does optimized invoking of the SIMD intrinsics.

## 2. Embree Tutorial

### 2.2 Prerequisites

- We explain with C++11 but not C11, for easiness of programming and its design.
- We follows Embree ver. 3.6.1 API.
- We does not support for installation. Please get dependency files from Releases page of Embree's GitHub repository and install them on your computer.

### 2.3 Sample 3D Model

We use a model for example to render like following.

```
vertex[0] = { -0.5f,  0.0f,  0.0f };
vertex[1] = { -0.0f,  1.0f,  0.0f };
vertex[2] = {  0.5f,  0.0f,  0.0f };
vertex[3] = {  0.0f, -1.0f,  0.0f };
vertex[4] = {  0.0f,  0.0f,  0.5f };
vertex[5] = {  0.0f,  0.0f, -0.5f };
index[0] = { 1, 0, 4 };
index[1] = { 1, 4, 2 };
index[2] = { 3, 0, 4 };
index[3] = { 3, 4, 2 };
index[4] = { 1, 5, 0 };
index[5] = { 1, 2, 5 };
index[6] = { 3, 5, 0 };
index[7] = { 3, 2, 5 };
```

The vertex buffer contains 3D points and the index buffer describes sets of indices of polygons.

[image of sample 3D model]
Figure 6. Sample 3D model for rendering.

### 2.4 Embree Objects

In Emblee, there are special objects that have particular roles of the API. Instances of types corresponding the Embree objects are dynamic-allocated by `rtcNewXXX` calls (XXX is name of Embree objects.). The functions return instances of types that are alias of integers as handling number. 

Embree objects can be managed by reference counting. Right after call of `rtcNewXXX`, the count is 1. When `rtcRetainXXX` called, the count gets +1. While, `rtcReleaseXXX` makes the count -1. If the count is 0, allocated instances are freed.

If you write wrapper classes for Embree objects, constructors and destructors may make the allocation/freeing management easier.

### 2.4 Programming Flow

Embree API calls must be written in particular order. See figure 6.

In this section, we show as following:

```cpp
// name of header file that declares structs
struct Foo;
```

```cpp
Hoge CreateHoge();
```

[Figure of order of Embree API call]
Figure 7. Order of Embree API call.

### 2.5 Device Generating

First of all, generate a **device**, which is an Embree object.
One can manage this instance by return value as handling number.

```C++
// embree3/rtcore_device.h
struct RTCDevice; // as handling number
RTCDevice rtcNewDevice(const char* config);
```

```C++
RTCDevice device = rtcNewDevice(nullptr);
```

The instantiated device is used to generate other Embree objects.
If specific string is handed to the argument, one can configure the kernel of Embree API. While `nullptr` tells the kernel default configuration.

```c++
RTCDevice device = rtcNewDevice("threads=1,isa=avx"); // multiple configuration by comma separating.
```

Otherwise, one can specify configuration by other ways.
If you did more than one, greater number is prior to configure.

1. string taken as argument of  `rtcNewDevice`.
2. `.embree` file that placed in same directory of developed application.
3. `.embree` file that placed in home directory.

For more detail of configuration, see `rtcNewDevice` of the official Embree reference.

### 2.6 Scene Initialization

Next, generate a **scene**, which is an Embree object.
A scene will contain "3D model instances" in virtual space.

```C++
// embree3/rtcore_scene.h
struct RTCScene; // as handling number
RTCScene rtcNewScene(RTCDevice device);
```

```C++
RTCScene scene = rtcNewScene(device);
```

"3D model instances" will be atached to a scene and the scene will be used in raytracing part.

### 2.7 Geometry Initialization

#### 2.7.1 Geometry Generating

Next, generate a geometry, also known as "3D model instances".
This is also an Embree object.

```C++
// embree3/rtcore_geometry.h
struct RTCGeometry; // as handling number
RTCGeometry rtcNewGeometry(RTCDevice device, RTCGeometryType type);
```

```C++
RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
```

Enum values of Embree `RTCGeometryType` are taken as second argument of `rtcNewGeometry`.
`RTCGeometryType` tells Embree kernel how buffer for this "3D model instance" is allocated.

For example, one of a value `RTC_GEOMETRY_TYPE_TRIANGLE` represents polygonal mesh and tells such allocation to the kernel.

#### 2.7.2 Buffer Allocation and Geometry Binding

Next, allocate for "3D model instances" and bind them to geometries.
There are two ways to dynamic-allocate: 

- allocate by Embree API and get its head address
- allocate by programmer and tell its head address to Embree kernel

```C++
// embree3/rtcore_geometry.h
/*
  Allocation by Embree API
*/
void* rtcSetNewGeometryBuffer(
  RTCGeometry geometry,    // a geometry previously generated
  enum RTCBufferType type, // what this buffer represents
  unsigned int slot,       // 
  enum RTCFormat format,   // Type and number of variables that an element contains
  size_t byteStride,       // size of an element
  size_t itemCount         // number of elements
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

`rtcSetNewGeometryBuffer` internally calculates buffer size and allocates.
In the example above, get an address from the `void*` return value and utilize it as an array.

```C++
// embree3/rtcore_geometry.h
/*
  allocation by programmer
*/
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
struct PolygonVertex {
  float x, y, z;
};
struct PolygonIndex{
  unsigned int v0, v1, v2;
}
// ~中略~
constexpr size_t numVertex = 6;
constexpr int numPolygons = 8;
std::array<PolygonVertex, numVertex> geometryVertices;
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

`rtcSetSharedGeometryBuffer` tells already-allocated buffer's address to Embree kernel. The buffer can be utilized as geometry instance.
In the example above, tell address of `geometryVertices` defined as `std::array<PolygonVertex, numVertex>` to Embree kernel.

#### 2.7.3 Data Input to Buffer

Next, input data to allocated geometry buffer.
This process can be replaced into 3D model loading.
If allocation by `rtcSetSharedGeometryBuffer`, data input can be done when definition of `geometryVertices`.

```C++
// No Embree API used.
```

```C++
// model data from 3.2.2
geometryVertices[0] = {-0.5f,  0.0f,  0.0f};
geometryVertices[1] = {-0.0f,  1.0f,  0.0f};
geometryVertices[2] = { 0.5f,  0.0f,  0.0f};
geometryVertices[3] = { 0.0f, -1.0f,  0.0f};
geometryVertices[4] = { 0.0f,  0.0f,  0.5f};
geometryVertices[5] = { 0.0f,  0.0f, -0.5f};
geometryPolygons[0] = { 1, 0, 4 };
geometryPolygons[1] = { 1, 4, 2 };
geometryPolygons[2] = { 3, 0, 4 };
geometryPolygons[3] = { 3, 4, 2 };
geometryPolygons[4] = { 1, 5, 0 };
geometryPolygons[5] = { 1, 2, 5 };
geometryPolygons[6] = { 3, 5, 0 };
geometryPolygons[7] = { 3, 2, 5 };
```

#### 2.7.4 Seometry Commiting

After allocation of geometry buffer and data input, tell geometry state to Embree kernel.

```C++
// embree3/rtcore_geometry.h
void rtcCommitGeometry(RTCGeometry geometry);
```

```c++
rtcCommitGeometry(geometry);
```

Everytime buffers' value (in this example, `geometryVertices` and/or `geometryPolygons`) are changed, this function should be called. If no call, raytracing rendering result is no longer changed.
For example, if you would like to implement object movement in realtime rendering, this function should be called in every frame.

### 2.8 Geometry Attaching to Scene

Register geometry as initialized in the previous step.
Multiple geometries can be attached to a scene.

```C++
// embree3/rtcore_scene.h
unsigned int rtcAttachGeometry(
  RTCScene scene,
  RTCGeometry geometry
);
```

```C++
rtcAttachGeometry(scene, geometry);
```


After registration, one can release the Embree geometry object.

```C++
// embree3/rtcore_geometry.h
void rtcReleaseGeometry(RTCGeometry geometry);
```

```c++
rtcReleaseGeometry(geometry);
```

This function call means that programmer does not need to manage the geometry object because Embree kernel gets reference to the geometry object.
Geometry releasing is not necessary in this time, but required before terminating Embree kernel.

### 2.9 Scene construction

Tell current state of scene to Embree kernel.

```C++
// declared in embree3/rtcore_scene.h
void rtcCommitScene(RTCScene scene);
```

```C++
rtcCommitScene(scene);
```

When this function is called, Embree kernel internally constructs an instance of a special data structure for raytracing, called Bounding Volume Hierarchy. BVH is also an Embree object. For more detail of BVH, please see [BVH REFERENCE].

Setup for raytracing is fully completed in this step.

### 2.10 Casting a ray

```C++
// embree3/rtcore_common.h
struct RTCIntersectContext;
void rtcInitIntersectContext(RTCIntersectContext* context);

// embree3/rtcore_ray.h
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

RTCRayHit rayhit;
rayhit.ray.org_x = 0.0f;
rayhit.ray.org_y = 0.0f;   // ray origin: (0, 0, -10)  
rayhit.ray.org_z = -10.0f; 
rayhit.ray.dir_x = 0.0f;
rayhit.ray.dir_y = 0.0f;   // ray casting direction: (0, 0, 1)
rayhit.ray.dir_z = 1.0f;
rayhit.ray.tnear = 0.0f;          // from 0.0 distance point from the origin,
rayhit.ray.tfar = FLOAT_INFINITY; // cast a ray for inf. distance.

rtcIntersect1(sceneHandle, &context, &rayhit);

if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
    // has intersected to something of the scene
}
else{
    // has not intersected
}
```

Generate an `RTCRayHit` instance and check whether a ray hits something by `rtcIntersect1`.
After `rtcIntersect1` call, geometry ID of a closest object that was intersected the ray is written to `rayhit.hit.geomID`.
The direction values (`RTCRayHit::dir_x` s) are not required to be normalized.

### 2.11 Raytracing with Screen

For easiness, consider rendering like parallel projection of rasterization. 
For example, render to 100x100 pixels screen to be rendered.

[Figure of parallel raytracing]
Figure 8. Raytracing like parallel projection of the rasterization algorithm. 

### 2.12 Termination

Release all Embree objects that programmer is managing. 

```C++
// embree3/rtcore_scene.h
void rtcReleaseScene(RTCScene scene);

// rtcore_device.h
void rtcReleaseDevice(RTCDevice device);
```

```C++
rtcReleaseGeometry(geometryHandle); // If you have not done in 3.2.7.
rtcReleaseScene(sceneHandle);
rtcReleaseDevice(deviceHandle);
```

### 2.13 Whole Raytracing Code

Whole code that renders to .ppm file, as well as 3.2.10.
Figure 9 is rendering result of this code.

```C++
#include <embree3/rtcore.h>
#include <array>
#include <vector>
#include <limits>
#include <fstream>

struct Vertex {
  float x, y, z;
};

struct PolygonIndex {
  unsigned int v0, v1, v2;
};

struct ColorRGB {
  uint8_t r, g, b;
};

constexpr uint32_t SCREEN_WIDTH_PX = 100;
constexpr uint32_t SCREEN_HEIGHT_PX = 100;

void SaveAsPpm(const std::vector<std::array<ColorRGB, SCREEN_WIDTH_PX>>& image, const char* const fileName) {
  std::ofstream ofs(fileName);

  if (!ofs) {
    std::exit(1);
  }

  ofs << "P3\n"
      << SCREEN_WIDTH_PX << " " << SCREEN_HEIGHT_PX << "\n255\n";
  for (const auto& row : image) {
    for (const auto& pixel : row) {
      ofs << static_cast<int>(pixel.r) << " "
          << static_cast<int>(pixel.g) << " "
          << static_cast<int>(pixel.b) << "\n";
    }
  }

  ofs.close();
}

int main() {
  RTCDevice deviceHandle = rtcNewDevice(nullptr);
  RTCScene sceneHandle = rtcNewScene(deviceHandle);
  RTCGeometry geometryHandle = rtcNewGeometry(deviceHandle, RTC_GEOMETRY_TYPE_TRIANGLE);

  constexpr size_t numVertices = 6;
  constexpr size_t numPolygons = 8;

  std::array<Vertex, numVertices> geometryVertices;
  std::array<PolygonIndex, numPolygons> geometryPolygons;

  rtcSetSharedGeometryBuffer(geometryHandle,
                             RTC_BUFFER_TYPE_VERTEX,
                             0,
                             RTC_FORMAT_FLOAT3,
                             &geometryVertices,
                             0,
                             sizeof(Vertex),
                             geometryVertices.size());

  geometryVertices[0] = { -0.5f,  0.0f,  0.0f };
  geometryVertices[1] = {  0.0f,  1.0f,  0.0f };
  geometryVertices[2] = {  0.5f,  0.0f,  0.0f };
  geometryVertices[3] = {  0.0f, -1.0f,  0.0f };
  geometryVertices[4] = {  0.0f,  0.0f, -0.5f };
  geometryVertices[5] = {  0.0f,  0.0f,  0.5f };

  rtcSetSharedGeometryBuffer(geometryHandle,
                             RTC_BUFFER_TYPE_INDEX,
                             0,
                             RTC_FORMAT_UINT3,
                             &geometryPolygons,
                             0,
                             sizeof(PolygonIndex),
                             geometryPolygons.size());

  geometryPolygons[0] = { 1, 0, 4 };
  geometryPolygons[1] = { 1, 4, 2 };
  geometryPolygons[2] = { 3, 0, 4 };
  geometryPolygons[3] = { 3, 4, 2 };
  geometryPolygons[4] = { 1, 5, 0 };
  geometryPolygons[5] = { 1, 2, 5 };
  geometryPolygons[6] = { 3, 5, 0 };
  geometryPolygons[7] = { 3, 2, 5 };

  rtcCommitGeometry(geometryHandle);
  rtcAttachGeometry(sceneHandle, geometryHandle);
  rtcReleaseGeometry(geometryHandle);
  rtcCommitScene(sceneHandle);

  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
  std::vector<std::array<ColorRGB, SCREEN_WIDTH_PX>> screen{SCREEN_HEIGHT_PX};

  RTCIntersectContext context;
  rtcInitIntersectContext(&context);

  RTCRayHit rayhit;
  rayhit.ray.org_z = -10.0f;
  rayhit.ray.dir_x = 0.0f;
  rayhit.ray.dir_y = 0.0f;
  rayhit.ray.dir_z = 1.0f;
  rayhit.ray.tnear = 0.0f;

  constexpr uint32_t HALF_HEIGHT = static_cast<int>(SCREEN_HEIGHT_PX) / 2;
  constexpr uint32_t HALF_WIDTH = static_cast<int>(SCREEN_WIDTH_PX) / 2;

  for (int32_t y = 0; y < SCREEN_HEIGHT_PX; ++y) {
    for (int32_t x = 0; x < SCREEN_WIDTH_PX; ++x) {
      rayhit.ray.org_x = (x - static_cast<int>(HALF_WIDTH)) / static_cast<float>(HALF_WIDTH);
      rayhit.ray.org_y = (y - static_cast<int>(HALF_HEIGHT)) / static_cast<float>(HALF_HEIGHT);
      rayhit.ray.tfar = FLOAT_INFINITY;
      rayhit.ray.flags = false;
      rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
      rtcIntersect1(sceneHandle, &context, &rayhit);
      if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        screen[y][x].r = 0xff;
        screen[y][x].g = 0x00;
        screen[y][x].b = 0xff;
      }
      else {
        screen[y][x].r = 0x00;
        screen[y][x].g = 0x00;
        screen[y][x].b = 0x00;
      }
    }
  }

  SaveAsPpm(screen, "result.ppm");

  rtcReleaseScene(sceneHandle);
  rtcReleaseDevice(deviceHandle);
}
```

[Rendering result image]
Figure 9. Parallel rendering result of the hexahedron model.

## 3 Renderer Implementation

To switch rendering methods, the logic part of rendering should be brought out as a function, then one can switch rendering methods by conditional branching or otherwise polymorphism. 

We do not refer to polymorphism and  implementation of specific rendering methods in this research.

Rendering of a pixel can be considered independent in raytracing, so inside of the double loop of 3.2.12 should be cut and pasted as following pseudo-code:

```
RenderMethodA(scene, origin, castDirection){
  // Renders a pixel by render method A
}
RenderMethodB(scene, origin, castDirection){
  // Renders a pixel by render method B
}
```

```
renderCond = user input
image = initialized image
for each row of image{
  for each pixel of a row{
    origin = compute raytracing start point
    dir = compute first raytracing direction
    if(renderCond == METHOD_A){
      image[row][pixel] = RenderMethodA(scene, origin, dir)
    }
    else if (renderCond == METHOD_B){
      image[row][pixel] = RenderMethodB(scene, origin, dir)
    }
  }
}
```

# 4. Result

We implemented a renderer based on this tutorial. Plaese see [CGRS GitHub] to know implemented program. Rendering results, also indicated in the repository, are figure 10.

[Two images rendered by different methods]
Figure 10. Rendering by phong shading [PHONG SHADING REFERENCE] and pathtracing.

# 4. Discussion

We could obtain two images of same scene, same resolution, from same angle, by different methods. As mentioned in 1.2, same condition of image is necessary to apply subtraction. 

Figure 11 shows actual subtraction of the two images indicated in figure10. We now could know where areas are incorrectly rendered in the scene. When debugging rendering program, knowing strangely-rendered locations is important to determine what the program problems are.

[Subtraction of two images]
Figure 11. Subtraction of phong shading and pathtracing. Great difference is observed beacause phong shading is less effective PBR method than pathtracing in viewpoint of rendering correctness.

# References

[Embree Reference] https://www.embree.org/index.html

[Embree GitHub] https://github.com/embree/embree

[Shirotsu Qiita] https://qiita.com/Nao-Shirotsu/items/82e828666c1a6f2501e1