PBRT-v3 implementation of LFRayTracer
===============

Roughly 98% of this code is taken from magnificent [PBRT-v3 renderer](https://github.com/mmp/pbrt-v3), and thus 98% of credits go to PBRT-v3 authors.
Rest of 2% credits go to the owner of this repository.
The authors contribution is in making the original PBRT-v3 renderer compatible with LFRayTracer interface class (see [src/LFRayTracer.h](https://github.com/LeksiDor/LFRayTracerPBRT/blob/master/src/LFRayTracer.h)).


What is it for?
-------------

This project basically implements (almost) pure virtual [LFRayTracer](https://github.com/LeksiDor/LFRayTracerPBRT/blob/master/src/LFRayTracer.h) interface class.
LFRayTracer interface allows very high user-defined customization of ray/sample generation/accumulation, which is particularly useful for designing the rendering algorithms for non-conventional displays.
The main usage of this code can be found in [LFDisplay](https://github.com/LeksiDor/LFDisplay) project, which is about simulation of various light field displays.



Building the code
-------------

To check out the code together with all dependencies, be sure to use the `--recursive` flag when cloning the repository, i.e.
```bash
$ git clone --recursive https://github.com/LeksiDor/LFRayTracerPBRT.git
```

The code uses CMake as the build system.
Original PBRT-v3 is claimed to run on Windows, Linux with other Unixes, and Mac.
The current code, however, was tested only on Windows 10 machines.
Supporting it for other platforms is currently not a priority for the author, but everything can change.

The building process itself is typical for CMake-based projects.
Probably, the only extra variable you should put attention to is `PBRT_FLOAT_AS_DOUBLE`, which determines what Real type is used (true for double, false for float).
