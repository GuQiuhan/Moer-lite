#pragma once
#include "../NDF/NDF.h"
#include "BSDF.h"
#include "Warp.h"

class RoughConductorBSDF : public BSDF {
public:
  RoughConductorBSDF(const Vector3f &_normal, const Vector3f &_tangent,
                     const Vector3f &_bitangent, Spectrum _albedo,
                     Vector2f _alpha, Vector3f _eta, Vector3f _k, NDF *_ndf)
      : BSDF(_normal, _tangent, _bitangent), albedo(_albedo), alpha(_alpha),
        eta(_eta), k(_k), ndf(_ndf) {}

  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    Spectrum specular{0.f};
    // TODO
    // 1. 转换坐标系到局部坐标
    Vector3f wout=toLocal(wo);
    Vector3f win=toLocal(wi);//入射光改为从外向交点


    // 2. 根据公式计算 Fr, D, G
    float D,G;
    Vector3f wh=(wout+win)/(wout+win).length();//微观表面法线
    D=ndf->getD(wh,alpha);
    G=ndf->getG(wout,win,alpha);

    //计算Fr
    float thetaI=acos(dot(normalize(win),Vector3f(0,1,0)));
    float thetaO=acos(dot(normalize(wout),Vector3f(0,1,0)));
    Vector3f Fr=getFr(cos(thetaO));
    // 3. return albedo * D * G * Fr / (4 * \cos\theta_o);
    // tips: brdf
    // 中分母的\cos\theta_i项被渲染方程中的cos项消去，因此分母只有4*\cos\theta_o
    specular=albedo*D*G*Fr/(4*cos(thetaO));
    return specular;
  }

  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    Vector3f wi = squareToCosineHemisphere(sample);
    float pdf = squareToCosineHemispherePdf(wi);
    return {f(wo, toWorld(wi)) / pdf, toWorld(wi), pdf, BSDFType::Diffuse};
  }

  Vector3f getR0() const noexcept {
    return ((eta - 1.f) * (eta - 1.f) + k * k) /
           ((eta + 1.f) * (eta + 1.f) + k * k);
  }
  Vector3f getFr(float cosTheta) const noexcept {
    Vector3f r0 = getR0();
    return r0 + (Vector3f(1.f) - r0) * std::pow(1.f - cosTheta, 5.f);
  }

private:
  Spectrum albedo;
  Vector2f alpha;
  Vector3f eta;
  Vector3f k;
  NDF *ndf;
};