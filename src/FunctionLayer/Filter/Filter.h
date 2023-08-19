#pragma once
#include <ResourceLayer/Image.h>
#include <memory>
#include <vector>

// 实现传统滤波器

class Filter {
public:
  void MedianFilter(const std::shared_ptr<Image>& src, std::shared_ptr<Image>& dst,double size=21);
  void MeanFilter(const std::shared_ptr<Image>& src,std::shared_ptr<Image>& dst,double size=3);
  void GaussianFilter(const std::shared_ptr<Image>& src, std::shared_ptr<Image>& dst,double size=9, double sigma=0.8);
  std::vector<std::vector<double>> GaussianTemplate(double size, double sigma);
  void BilateralFilter(const std::shared_ptr<Image>& src, std::shared_ptr<Image>& dst, int size=9, float sigmaD=0.8, float sigmaR=65);
  std::vector<float> RangeTemplate(int range, float sigmaR);


private:
    //static constexpr int size = 3; //默认滤波窗口的大小为3x3

};


