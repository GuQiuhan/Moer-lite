#pragma once
#include "BSDF.h"
#include "Warp.h"
#include <algorithm>

class OrenNayarBSDF : public BSDF {
public:
  OrenNayarBSDF(const Vector3f &_normal, const Vector3f &_tangent,
                const Vector3f &_bitangent, Spectrum _albedo, float _sigma)
      : BSDF(_normal, _tangent, _bitangent), albedo(_albedo), sigma(_sigma) {}

  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    Spectrum diffuse{0.f};
    // TODO
    // 1. 转换坐标系到局部坐标
    Vector3f win=toLocal(wi);//这里表示光照方向，是入射方向的反方向
    Vector3f wout=toLocal(wo);
    
    // 2. 计算 A, B, \alpha, \beta（可以直接求\sin\alpha,\tan\beta）, \cos(\phi_i-\phi_o)
    float A=1-(pow(sigma,2)/(2*(pow(sigma,2)+0.33)));
    float B=(0.45*pow(sigma,2))/(pow(sigma,2)+0.09);
    float cosine=0.f,sinAlpha=0.f,tanBeta=0.f;

    float thetaI=acos(dot(normalize(win),Vector3f(0,1,0)));
    float thetaO=acos(dot(normalize(wout),Vector3f(0,1,0)));
    if(thetaI>thetaO) {sinAlpha=sin(thetaI); tanBeta=tan(thetaO);}
    else {sinAlpha=sin(thetaO); tanBeta=tan(thetaI);}

    float phiI=atan2(win[2],win[0]);//球坐标，y轴向上
    float phiO=atan2(wout[2],wout[0]);
    cosine=cos(phiI-phiO);
    if(cosine<0) cosine=0;


    // 3. return Oren-Nayar brdf
    diffuse=albedo*INV_PI*(A+B*cosine*sinAlpha*tanBeta);
    return diffuse*cos(thetaI);//注意最后要乘以cos
  }

  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    Vector3f wi = squareToCosineHemisphere(sample);
    float pdf = squareToCosineHemispherePdf(wi);
    return {albedo, toWorld(wi), pdf, BSDFType::Diffuse};
  }

private:
  Spectrum albedo;
  float sigma;
};
