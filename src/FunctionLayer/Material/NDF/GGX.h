#pragma once

#include "NDF.h"

class GGXDistribution : public NDF {
public:
  GGXDistribution() noexcept = default;
  virtual ~GGXDistribution() noexcept = default;
  virtual float getD(const Vector3f &whLocal,
                     const Vector2f &alpha) const noexcept override {
    // TODO
    // 根据公式即可
    float theta=acos(dot(normalize(whLocal),Vector3f(0,1,0)));
    return INV_PI*dot(alpha,alpha)/(pow(cos(theta),4)*pow(dot(alpha,alpha)+tan(theta)*tan(theta),2));
   
    //return 0.f;
  }
  // tips:
  // float getG1(...) {}
  virtual float getG(const Vector3f &woLocal, const Vector3f &wiLocal,
                     const Vector2f &alpha) const noexcept override {
    // TODO
    // 根据公式即可
    // tips: return getG1(wo) * getG1(wi);
    float gi=1,go=1;
    //Vector3f wh=(woLocal+wiLocal)/(woLocal+wiLocal).length();//微观表面法线
    float thetaI=acos(dot(Vector3f(0,1,0),normalize(wiLocal)));
    float thetaO=acos(dot(Vector3f(0,1,0),normalize(woLocal)));
    
    gi=2/(1+sqrt(1+dot(alpha,alpha)*pow(tan(thetaI),2)));
    go=2/(1+sqrt(1+dot(alpha,alpha)*pow(tan(thetaO),2)));
   
    return gi*go;
    //return 0.f;
  }
  virtual float pdf(const Vector3f &woLocal, const Vector3f &whLocal,
                    const Vector2f &alpha) const noexcept override {
    return getD(whLocal, alpha) * whLocal[1];
  }
  virtual Vector3f sampleWh(const Vector3f &woLocal, const Vector2f &alpha,
                            const Vector2f &sample) const noexcept override {
    float a = alpha[0];
    float tan_theta_2 = a * a * sample[0] / (1.f - sample[0]);
    float phi = sample[1] * 2 * PI;

    float cos_theta = std::sqrt(1.f / (1.f + tan_theta_2));
    float sin_theta = std::sqrt(1.f - cos_theta * cos_theta);
    return {sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
  }
};