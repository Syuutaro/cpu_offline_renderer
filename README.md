# CPU Offline Renderer
* This application reads a JSON format scene file and generates images by ray tracing.
![image1](/sample1/result/spp1000.png)
![image2](/sample2/result/spp1000.png)

# Feature
* [x] Integrator
    * [x] Path Tracing
    * [ ] Path Tracing with Multiple Importance Sampling
    * [ ] Bidirectional Path Tracing

* [x] Acceleration Structure
    * [x] SAHBVH
    * [x] LBVH

* [x] BSDF
    * [x] Ideal Diffuse
    * [x] Ideal Conductor
    * [x] Ideal Dielectric
    * [x] Smith GGX Rough Diffuse
    * [x] Smith GGX Rough Conductor
    * [x] Smith GGX Rough Dielectric
    * [x] Heitz VNDF Importance Sampling using ellipsoid

* [x] Camera
    * [x] Pinhole Model
    * [x] Thinlens Model

* [x] Scene File
    * [x] JSON (ECMA-404 2nd Edition except hexadecimal digits and unicode)
    * [ ] Original Script Language

* [x] 3D Model Importing
    * [x] OBJ (only "v", "vt, "f" are available)

* [x] Multithreads




# Caution
* Texture format depends on the library stb.
* Application assumes texture as RGB texture.
* If you set a texture for a 3 component vector value, RGB channels are used.
* If you set a texture for a scalar value, R channel is used.
* Result image is png format.
* right handed coordinate system, z-axis up, v-axis up(texture coordinate)

# External Library
* stb


# Reference
* Eric Heitz,
"Sampling the GGX Distribution of Visible Normals",
JCGT 2018
* Eric Heitz, Johannes Hanika, Eugene d'Eon and Carsten Dachsbacher,
"Multiple-Scattering Microfacet BSDFs with the Smith Model",
ACM SIGGRAPH 2016
* Eric Heitz, Jonathan Dupuy, Cyril Crassin and Carsten Dachsbacher,
"SGGX Microflake Distribution",
ACM SIGGRAPH 2015
* Eric Heitz and Jonathan Dupuy,
"Implementing a Simple Anisotropic Rough Diffuse Material with Stochastic Evaluation",
Technical report 2015
* Eric Heitz and Eugene d'Eon,
"Importance Sampling Microfacet-Based BSDFs using Distribution of Visible Normals",
EGSR 2014
* Eric Heitz,
"Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs",
JCGT 2014
* Bruce Walter, Stephen R. Marschner, Hongsong Li, Kenneth E. Torrance,
"Microfacet Models for Refraction through Rough Surfaces",
EGSR 2007


# How to build
* execute bash script "build.bash" or create build script for your environment
* build test
    * MacOS Catalina version 10.15.3 + Xcode version 11.3.1


# How to use
1. Create a directory which has scene file "scene.json", directory "result", directory "model", and directory "texture".
If you use obj files or image files, you have to store each kinds of files in the directory "model" or "texture".
2. When executing the application, you can see comments "Input absolute path to asset directory" in command line,
so please input absolute path of your scene directory and press enter key.
3. Rendering images are generated and stored in asset_directory/result.

```
//hierarchical structure of asset directory
- asset_directory(any name)
    - scene.json (json file,required)
    - result (directory,required)
    - model (directory,option)
        - model1.obj
        - model2.obj
        - ...
        - ...
    - texture (directory,option)
        - image1.png
        - image2.png
        - ...
        - ...
```


# How to write scene file
* Scene file begins with JSON Object and the root JSON Object has the following keys.

1. model(option)
2. texture(option)
3. bsdf(required)
4. light(required)
5. object(required)
6. camera(required)
7. acceleration(required)
8. integrator(required)
9. rendering(required)

```
//scene.json
{
    "model": ...,
    "texture": ...,
    "bsdf": ...,
    "light": ...,
    "object": ...,
    "camera": ...,
    "acceleration": ...,
    "integrator": ...,
    "rendering": ...
}
```

1. model
* id
```
"id":"any id"
```
* file_name
```
"file_name":"obj file name stored in scene_directory/model"
```

```
//example
"model":[
    {
        "id":"model1",  
        "file_name":"model1.obj"
    },
    {
        "id":"model2",  
        "file_name":"model2.obj"
    }
]  
```

2. texture
* id
```
"id":"any id"
```
* file_name
```
"file_name":"image file name stored in scene_directory/texture"
```
```
//example
"texture":[
    {
        "id":"checker_board1",  
        "file_name":"checker_board1.png"
    },
    {
        "id":"checker_board2",  
        "file_name":"checker_board2.jpg"
    }
]  
```

3. bsdf
* id
```
"id":"any id"
```

* type
    * ideal_diffuse
    * ideal_conductor
    * ideal_dielectric
    * rough_diffuse
    * rough_conductor
    * rough_dielectric
```
"type":"ideal_diffuse"
"type":"ideal_conductor"
"type":"ideal_dielectric"
"type":"rough_diffuse"
"type":"rough_conductor"
"type":"rough_dielectric"
```

* reflectance or reflectance_texture
    * required for all type
```
"reflectance":[0~1 real number,0~1 real number,0~1 real number]
"reflectance_texture":"texture id you set in texture section"
```

* inside_eta,outside_eta
    * required for type "ideal_dielectric" or "rough_dielectric"
    * refractive index of inside and outside of surface
```
"inside_eta":positive real number
"outside_eta":positive real number
```

* anisotropy_x or anisotropy_x_texture
    * required for type "rough_xxx"
    * one of the parameter of GGX normal distribution
```
"anisotropy_x":positive real number
"anisotropy_x_texture":"texture id you set in texture section"
```

* anisotropy_y or anisotropy_y_texture
    * required for type "rough_xxx"
    * one of the parameter of GGX normal distribution
```
"anisotropy_y":positive real number
"anisotropy_y_texture":"texture id you set in texture section"
```

* tangent_texture
    * tangent vector map for tangent space
    * used for sampling GGX normal distribution
```
"tangent_texture":"texture id you set in texture section"
```

```
//example
"bsdf":[
    {
        "id":"ideal_diffuse_blue",  
        "type":"ideal_diffuse",
        "reflectance":[0,0,1]
    },
    {
        "id":"ideal_conductor_gold",  
        "type":"ideal_conductor",
        "reflectance":[0.85,0.7,0]
    },
    {
        "id":"ideal_dielectric_white",
        "type":"ideal_dielectric",
        "reflectance":[1,1,1],
        "inside_eta":1.7,
        "outside_eta":1.0
    },
    {
        "id":"rough_diffuse_white",  
        "type":"rough_diffuse",
        "reflectance":[1,1,1],
        "anisotropy_x":0.1,
        "anisotropy_y":1.0
    },
    {
        "id":"rough_conductor_gold",  
        "type":"rough_conductor",
        "reflectance_texture":"pattern1_texture"
        "anisotropy_x":0.1,
        "anisotropy_y_texture":"pattern2_texture",
        "tangent_texture":"tangent_texture"
    },
    {
        "id":"rough_dielectric_white",  
        "type":"rough_dielectric",
        "reflectance":[1,1,1],
        "inside_eta":1.7,
        "outside_eta":1.0,
        "anisotropy_x":0.1,
        "anisotropy_y":0.1
    }
]
```

4. light
* id
```
"id":"any id"
```

* radiance or radiance_texture
```
"radiance":[0~1 real number,0~1 real number,0~1 real number]
"radiance_texture":"texture id you set in texture section"
```

* scale
    * scale of radiance
```
"scale":non negative number
```

```
//example
"light":[
    {
        "id":"white_light",  
        "radiance":[1,1,1],
        "scale":1
    },
    {
        "id":"blue_light",  
        "radiance_texture":"blue_texture",
        "scale":2
    }
]
```

5. object
* id
```
"id":"any id"
```

* type
    * triangle
    * parallelogram
    * sphere
    * mesh
```
"type":"triangle"
"type":"parallelogram"
"type":"sphere"
"type":"mesh"
```

* bsdf
    * required for all type
```
"bsdf":"bsdf id you set in bsdf section"
```

* light
    * required for all type
```
"light":"light id you set in light section"
```

* model
    * required for type "mesh"
```
"model":"model id you set in model section"
```

* origin
    * required for all type
    * origin of model's local coordinate in world coordinate
```
"origin":[real number,real number,real number]
```

* scale
    * required for type "triangle" or "parallelogram"
    * scale of model's local coordinate in world coordinate
```
"scale":[real number,real number,real number]
```

* attitude
    * required for all type
    * attitude of model's local coordinate in world coordinate
    * component 0~2 is z-axis of model's local coordinate in world coordinate
    * component 3 is rotation around the axis vector and degree measure
```
"attitude":[real number,real number,real number,real number]
```

* xyz1,xyz2,xyz3
    * required for type "triangle" or "parallelogram"
    * vertex position in local coordinate
    * xyz4 for type "parallelogram" is automatically calculated
```
"xyz1":[real number,real number,real number]
"xyz2":[real number,real number,real number]
"xyz3":[real number,real number,real number]
```

* uv1,uv2,uv3
    * required for type "triangle" or "parallelogram" with texture mapping
    * vertex position in texture coordinate
    * uv4 for type "parallelogram" is automatically calculated
```
"uv1":[0~1 real number,0~1 real number]
"uv2":[0~1 real number,0~1 real number]
"uv3":[0~1 real number,0~1 real number]
```

* radius
    * required for type "sphere"
```
"radius":positive real number
```

```
//example
"object":[
    {
        "id":"object1",
        "type":"triangle",
        "bsdf":"ideal_diffuse_white",
        "light":"black",
        "origin":[50,50,0],
        "scale":[20,20,20],
        "attitude":[0,0,1,45]
        "xyz1":[-1,-1,0],
        "xyz2":[1,-1,0],
        "xyz3":[1,1,0]
    },
    {
        "id":"object2",
        "type":"parallelogram",
        "bsdf":"ideal_conductor_gold",
        "light":"black",
        "origin":[0,50,50],
        "scale":[50,50,50],
        "attitude":[1,0,0,0],
        "xyz1":[-1,-1,0],
        "xyz2":[1,-1,0],
        "xyz3":[1,1,0],
        "uv1":[0,0],
        "uv2":[1,0],
        "uv3":[1,1]
    },
    {
        "id":"object3",
        "type":"sphere",
        "bsdf":"ideal_diffuse_white",
        "light":"white",
        "origin":[50,50,120],
        "attitude":[0,0,1,0],
        "radius":30
    },
    {
        "id":"object4",
        "type":"mesh",
        "bsdf":"rough_conductor_gold",
        "light":"black",
        "model":"cube",
        "origin":[50,50,20],
        "scale":[10,10,10],
        "attitude":[1,-1,1,120]
    }
]
```

6. camera
* type
    * pinhole
    * thinlens
```
"type":"pinhole"
"type":"thinlens"
```

* sensor_width,sensor_height
    * required for all type
    * pixel width and height of result image
```
"sensor_width":positive integer
"sensor_height":positive integer
```

* sensor_origin
    * required for all type
    * camera position in world coordinate
```
"sensor_origin":[real number,real number,real number]
```

* sensor_target
    * required for all type
    * camera looks at this target point
```
"sensor_target":[real number,real number,real number]
```

* rotation
    * required for all type
    * rotation around camera's z-axis
    * degree measure
    * used for calculating local coordinate of camera in world coordinate
```
"rotation":real number
```

* sensor_pinhole_length
    * required for type "pinhole"
    * distance between sensor_origin and pinhole
```
"sensor_pinhole_length":positive real number
```

* sensor_lens_length
    * required for type "thinlens"
    * distance between sensor_origin and lens's center
```
"sensor_lens_length":positive real number
```

* lens_radius
    * required for type "thinlens"
    * radius of disk type lens
```
"lens_radius":positive real number
```

```
//example1
"camera":{
    "type":"pinhole"
    "sensor_width":1280,
    "sensor_height":720,
    "sensor_origin":[50,5,30],
    "sensor_target":[50,50,20],
    "rotation":0,
    "sensor_pinhole_length":1.2
}

//example2
"camera":{
    "type":"thinlens"
    "sensor_width":1280,
    "sensor_height":720,
    "sensor_origin":[50,5,30],
    "sensor_target":[50,50,20],
    "rotation":60,
    "sensor_lens_length":1.0,
    "lens_radius":1
}
```

7. acceleration
* type
    * sahbvh
    * lbvh
```
"type:"sahbvh"
"type":"lbvh"
```

```
//example1
"acceleration":{
    "type":"sahbvh"
}

//example2
"acceleration":{
    "type":"lbvh"
}
```

8. integrator
* type
    * pt
```
"type":"pt"
```

```
//example
"integrator":{
    "type":"pt"
}
```

9. rendering
* sample_per_pixel
    * application runs until this count
* rendering_time
    * application runs until this time
    * unit of time is second
* rendering_interval
    * interval of generating result images based on current sample count per pixel
    * rendering_interval = 10 -> rendering images at current sample per pixel = 10,20,30...
    * rendering_interval = 15 -> rendering images at current sample per pixel = 15,30,45...
* thread_count
    * the number of thread application uses
```
//example
"rendering":{
    "sample_per_pixel":100,
    "rendering_time":3600,
    "rendering_interval":10,
    "thread_count:8
}
```



