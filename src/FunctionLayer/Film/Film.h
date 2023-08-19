#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <ResourceLayer/Image.h>
#include <ResourceLayer/JsonUtil.h>
#include <FunctionLayer/Filter/Filter.h>
#include <FunctionLayer/Noise/Noise.h>

class Film {
public:
  Film() = delete;

  Film(const Json &json) {
    size = fetchRequired<Vector2i>(json, "size");
    image = std::make_shared<Image>(size);
    deimage = std::make_shared<Image>(size);
  }

  void deposit(const Vector2i xy, const Spectrum &spectrum) {
    //* 无论光谱内部实现如何，写入图片时均转为3通道格式
    Vector3f v = toVec3(spectrum);
    image->setValue(xy, v);
  }

  void savePNG(const char *filename) { image->savePNG(filename); }

  void saveHDR(const char *filename) { image->saveHDR(filename); }
  
  void savePFM(const char *filename) { image->savePFM(filename); }

  void filter(const int type){
    if(type==1) Filter().MedianFilter(image,deimage);
    
    if(type==2) Filter().MeanFilter(image,deimage);

    if(type==3) Filter().GaussianFilter(image,deimage);

    if(type==4) Filter().BilateralFilter(image,deimage);
    
  }

  void addNoise(const int type){
    if(type==1) Noise().AddSaltNoise(image);
    
    if(type==2) Noise().AddGaussianNoise(image);   
  }

  void save_dePNG(const char *filename) { deimage->savePNG(filename); } //保存滤波后的图片

  void save_deHDR(const char *filename) { deimage->saveHDR(filename); }


public:
  Vector2i size;

protected:
  std::shared_ptr<Image> image = nullptr;
  std::shared_ptr<Image> deimage = nullptr; //降噪后的image
};