# Animaker
## A Simple Library To Make Simple Videos
+ Based Direct3D, Direct2D and DirectWrite
## How to use
+ Architecture
```
Renderer
 |-Surface
 |-GraphicsObject
 |-TextObject
```
+ Use
```C++
using Animaker::Core;
using Animaker::Math;
using Animaker::Animation;

int ret;
Animaker::Core Renderer renderer;
ret = renderer.Init();
if(ret!=RV_OK){
    exit(-1);
}

Surface surface;
ret = renderer.SurfaceInit(&surface);
if(ret!=RV_OK){
    exit(-1);
}
//todo
```
