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



How to use
-------------

Original PBRT-v3 renderer is kept in pbrt_exe target.
Example of usage LFRayTracer-based implementation is found in ExampleLFRayTracer target.

When you use the library in your custom project, you can call LFRayTracerPBRTInstance() to access the singleton LFRayTracer instance.
First, you should load a scene by calling LoadScene(filepath) method of LFRayTracer.
Then, you create three user-defined classes: RayGenerator, SampleGenerator, and SampleAccumulator.
If you don't want to create them, you can call CreateDefaultXXX method of LFRayTracer, which will return the corresponding PBRT-v3 analogue.
After you specified RayGenerator, SampleGenerator, and SampleAccumulator, you can call Render method.
Now it is completely up to you how to use the rendered result: no default write-to-file is provided, but you can easily do it, or show image to the screen if you like.

If you want to re-use the same scene with all PBRT-related rendering options, you can just modify RayGenerator, SampleGenerator, and/or SampleAccumulator, and then call Render method once again.
Alternatively, you can load another scene through LoadScene method, and do all the things you desire.
Finally, you should call LFRayTRacerPBRTRelease() method when you are done (please do it, but only once).
