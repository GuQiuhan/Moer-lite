# Moer-lite: Research Oriented Physically Based Renderer

![截屏2023-08-20 上午1.05.57](images/%E6%88%AA%E5%B1%8F2023-08-20%20%E4%B8%8A%E5%8D%881.05.57-2464762.png)

## Introduction

This is the curriculum lab of an elective course "The Rendering" offered by NJU for junior students. We build our own graphic renderer based on a teaching-oriented Monte Carlo path tracing framework, 

## Updates
- March 8, 2023 Complete lab0, compile and run the provided code, and get four rendered pictures.

- March 21st, 2023 Complete the basic part of lab. Realize the intersection of light with cone, cylinder and disk

- March 24th, 2023 Complete the bonus part of lab1. Realize and the construction and intersection of octree, and the accelerator is realized based on octree.

- March 25th, 2023 Complete the bonus part of lab1. Realize the construction and intersection of bvh,  the accelerator based on bvh (with some bugs). 

- May 7, 2023 Complete the material model.

- July 14th, 2023 Complete the modification of material model. 

- July 15th, 2023 Complete the realization and integration of four traditional filters. 

- July 16th, 2023 Complete the integration of  **Intel**® **Open Image Denoise**, but the script needs to be written again, and the command should be unified to complete the compilation integration and running integration. 

- July 20, 2023 Complete the multiple importance sampling. (Lack of multiple bounce, temporarily ineffective) 

- July 28th, 2023  Finish myScene1 modeled by myself.

  >- 2023-3-8 完成lab0，对提供的代码进行编译、运行，获得四张渲染出的图片
  >- 2023.3.21 完成lab1的base部分，实现了光线对于圆锥、圆柱、圆盘的求交
  >- 2023.3.24 完成lab1的bonus部分，实现了octree的构建和求交，基于octree实现了加速器
  >- 2023.3.25 完成lab1的bonus部分，实现bvh的构建和求交，基于bvh实现了加速器（有一点bug）
  >- 2023.5.7 完成材质模型
  >- 2023.7.14 完成材质模型的修改
  >- 2023 7.15 完成四种传统滤波的实现和集成
  >- 2023.7.16 完成**Intel**® **Open Image Denoise**的集成，但是需要再次编写脚本，统一命令，完成编译集成、运行集成
  >- 2023.7.20 完成多重重要性采样。（缺少多bounce，暂时无效果）
  >- 2023.7.28 完成自己创建的场景myScene1。

## Introduction to Moer-lite
### Features 

- Easy to deploy, "install & clone & make & run"
- Structured scene description using `json`
- Interface for integrator/medium/bxdf/texture... and some pre-set implementions
- Unworried resource management for heavy resource (image/mesh

### How to use

* Go to root

  ```cmake ..```

  ```make -j4```

* Go to `target` folder

  `./Moer <scene path> <add_noise> <denoise>`

### Examples
Five scene examples: 
- `examples/area-lights`：提供了一个面光源照亮一个Stanford bunny的场景；
- `examples/two-spotlights`：提供了一个有着两个点光源的场景；
- `examples/bunny`：提供了一个使用环境光（environment lighting）照亮Stanford bunny的场景。
- `examples/cornell-box`：提供了一个使用whitted-style ray tracing的cornell-box场景
- `examples/lab1-test0`：提供了一个简单的mesh场景用来进行求交测试


### Checklist

#### Accelerator
- [X] BVH
- [X] Embree
- [ ] others

#### Camera
- [x] pinhole
- [ ] thinlens
- [ ] spherical
- [ ] others

#### Integrator
- [x] path-tracing
- [ ] volpath-tracing
- [ ] bdpt
- [ ] mlt
- [ ] photon-mapping
- [ ] others

#### Sampler
- [X] independent 
- [X] stratified 
- [ ] low-discrepancy
  - [X] Halton
  - [ ] (0-2)-Sequence
  - [ ] Maximized minimal distance
  - [ ] Sobol'
- [ ] others

#### Emitter
- [X] point
- [X] area
- [X] infinite
- [X] infinite cap
- [ ] others

#### Media
- [X] homogeneous
- [ ] hetergeneous
- [ ] others

#### BxDF
- [X] lambert
- [X] mirror
- [X] dielectric
- [X] conductor
- [ ] disney
- [ ] plastic
- [ ] hair
- [ ] others

#### Shape
- [X] sphere
- [X] quad
- [X] triangle-mesh
- [X] curve
- [ ] others

#### Denoiser
- [X] mean filter
- [X] median filter
- [X] gaussian filter
- [X] bilateral filter
- [ ] others

#### Add noise
- [X] salt-and-pepper noise
- [X] gaussian noise
- [ ] others