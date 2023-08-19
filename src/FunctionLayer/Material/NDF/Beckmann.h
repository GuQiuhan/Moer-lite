#pragma once

#include "NDF.h"

class BeckmannDistribution : public NDF {
public:
  BeckmannDistribution() noexcept = default;
  virtual ~BeckmannDistribution() noexcept = default;
  virtual float getD(const Vector3f &whLocal,
                     const Vector2f &alpha) const noexcept override {
    // TODO
    // 根据公式即可
    //宏观法线就是(0,1,0)
    float theta=acos(dot(normalize(whLocal),Vector3f(0,1,0)));
    return INV_PI*exp((-1)*pow(tan(theta),2)/dot(alpha,alpha))/(dot(alpha,alpha)*pow(cos(theta),4));
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
    float ai=1/(alpha.len()*tan(thetaI));//？这里alpha.len()存疑
    float ao=1/(alpha.len()*tan(thetaO));
    if(ai<1.6) gi=(3.535*ai+2.181*ai*ai)/(1+2.276*ai+2.577*ai*ai);
    if(ao<1.6) go=(3.535*ao+2.181*ao*ao)/(1+2.276*ao+2.577*ao*ao);

    return gi*go;
  }
  virtual float pdf(const Vector3f &woLocal, const Vector3f &whLocal,
                    const Vector2f &alpha) const noexcept override {
    return getD(whLocal, alpha) * whLocal[1];
  }
  virtual Vector3f sampleWh(const Vector3f &woLocal, const Vector2f &alpha,
                            const Vector2f &sample) const noexcept override {
    float a = alpha[0];
    float tan_theta_2 = -std::log(1 - sample[0]) * a * a;
    float phi = sample[1] * 2 * PI;

    float cos_theta = std::sqrt(1.f / (1.f + tan_theta_2));
    float sin_theta = std::sqrt(1.f - cos_theta * cos_theta);
    return {sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
  }
};