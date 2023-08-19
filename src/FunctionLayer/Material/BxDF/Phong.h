#pragma once
#include "BSDF.h"
#include "Warp.h"

class PhongReflection : public BSDF {
public:
  PhongReflection(const Vector3f &_normal, const Vector3f &_tangent,
                  const Vector3f &_bitangent, Spectrum _kd, Spectrum _ks,
                    float _p)
        : BSDF(_normal, _tangent, _bitangent), kd(_kd), ks(_ks), p(_p) {}


  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    Spectrum diffuse{0.f};
    Spectrum specular{0.f};
    // TODO
    // 1. 转换坐标系到局部坐标
    Vector3f win=toLocal(wi);
    Vector3f wout=toLocal(wo);
    Vector3f wref=Vector3f(-win[0],win[1],-win[2]);//反射方向
    //Vector3f n=toLocal(normal);
    // 2. 根据公式计算 K_d, K_s
    
    diffuse=kd*dot(Vector3f(0,1,0),win);
    if(dot(wref,wout)<0) specular=0;
    else  specular=ks*pow(dot(wref,wout),p);//RGB的赋值：可以用另一个RGB来赋值，也可以用float来赋值

    // 3. return K_d + K_s
    return diffuse + specular;
    // tips:
    // Phong模型brdf实现不包括环境光项；其I/r^2项包含在光源采样步骤中，因此brdf中不包含I/r^2。
    
  }

  float pdf(const Vector3f &wo, const Vector3f &wi) const {
    Vector3f woLocal = toLocal(wo), wiLocal = toLocal(wi);
      return squareToCosineHemispherePdf(wiLocal);
  }

  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    Vector3f wiLocal = squareToCosineHemisphere(sample);
    auto wi = toWorld(wiLocal);
    auto bsdf_f = f(wo, wi);
    auto bsdf_pdf = pdf(wo, wi);
    return {bsdf_f / bsdf_pdf, wi, bsdf_pdf, BSDFType::Diffuse};
  }

private:
    Spectrum kd; // 漫反射系数
      Spectrum ks; // 高光（镜面反射）系数
  float p;                      // 高光衰减系数
};
