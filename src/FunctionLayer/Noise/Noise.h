#pragma once
#include <ResourceLayer/Image.h>
#include <memory>
#include <vector>

// 实现增加噪声

class Noise {
public:
  void AddSaltNoise(const std::shared_ptr<Image>& img, int n=10000);
  void AddGaussianNoise(const std::shared_ptr<Image>& img, double mu=0, double sigma=0.1);
};