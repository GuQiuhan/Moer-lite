#include "Cone.h"
#include "ResourceLayer/Factory.h"

bool Cone::rayIntersectShape(Ray &ray, int *primID, float *u, float *v) const {
    //* todo 完成光线与圆柱的相交 填充primId,u,v.如果相交，更新光线的tFar
    //* 1.光线变换到局部空间
    Ray newRay=transform.inverseRay(ray);
    //* 2.联立方程求解
    Vector3f D(newRay.direction[0],newRay.direction[1],newRay.direction[2]);
    Vector3f V(0.f,0.f,-1.f);
    Vector3f CO(newRay.origin[0],newRay.origin[1],newRay.origin[2]-height);
    float A=pow(dot(D,V),2)-pow(cosTheta,2);
    float B=2*(dot(D,V)*dot(CO,V)-dot(D,CO)*pow(cosTheta,2));
    float C=pow(dot(CO,V),2)-dot(CO,CO)*pow(cosTheta,2);
    float t0,t1;
    if(!Quadratic(A,B,C,&t0,&t1)) return false;//无解

    //* 3.检验交点是否在圆锥范围内
    Point3f p0=newRay.at(t0);
    Point3f p1=newRay.at(t1);
    bool flag0=true;
    bool flag1=true;
    //检验t0
    if(t0<newRay.tNear||t0>newRay.tFar) flag0=false;
    if(p0[2]<0||p0[2]>height) flag0=false;
    float phi0=atan2(p0[1],p0[0]);//atan2（y，x）求的是y/x的反正切，其返回值为[-pi,+pi]之间的一个数
    if(phi0<0) phi0+=2*PI;
    if(phi0>phiMax) flag0=false;

    //检验t1
    if(t1<newRay.tNear||t1>newRay.tFar) flag1=false;
    if(p1[2]<0||p1[2]>height) flag1=false;
    float phi1=atan2(p1[1],p1[0]);//atan2（y，x）求的是y/x的反正切，其返回值为[-pi,+pi]之间的一个数
    if(phi1<0) phi1+=2*PI;
    if(phi1>phiMax) flag1=false;
    //* 4.更新ray的tFar,减少光线和其他物体的相交计算次数
    if(flag0)//取近的一个
    {
        ray.tFar=t0;
        * primID=0;
        * u =phi0/phiMax;
        * v =p0[2]/height;
    }

    else {
        if(!flag1) return false;
        ray.tFar=t1;
        * primID=0;
        * u =phi1/phiMax;
        * v =p1[2]/height;
    }
    
    return true;
    //* Write your code here.
    //return false;
}

void Cone::fillIntersection(float distance, int primID, float u, float v, Intersection *intersection) const {
    /// ----------------------------------------------------
    //* todo 填充圆锥相交信息中的法线以及相交位置信息

    //* 2.位置信息可以根据uv计算出，同样需要变换
    float phi=u*phiMax;
    float y=radius/(sqrt(1/(tan(phi)*tan(phi))+1));
    if(phi>PI)//三四象限
    {
        y=-y;
    }
    float x=y/tan(phi);
    Point3f M(x,y,v*height);
    intersection->position=transform.toWorld(M);

    //* 1.法线可以先计算出局部空间的法线，然后变换到世界空间
    Vector3f CM(M[0],M[1],M[2]-height);
    float cm=CM.length();
    Point3f K(0.f,0.f,height-cm/cosTheta);
    Vector3f KM(M[0]-K[0],M[1]-K[1],M[2]-K[2]);
    float len=KM.length();
    Vector3f normal(KM[0]/len,KM[1]/len,KM[2]/len);//归一化处理！！！
    intersection->normal=transform.toWorld(normal);
    //* Write your code here.
    /// ----------------------------------------------------


    intersection->shape = this;
    intersection->distance = distance;
    intersection->texCoord = Vector2f{u, v};
    Vector3f tangent{1.f, 0.f, .0f};
    Vector3f bitangent;
    if (std::abs(dot(tangent, intersection->normal)) > .9f) {
        tangent = Vector3f(.0f, 1.f, .0f);
    }
    bitangent = normalize(cross(tangent, intersection->normal));
    tangent = normalize(cross(intersection->normal, bitangent));
    intersection->tangent = tangent;
    intersection->bitangent = bitangent;
}

void Cone::uniformSampleOnSurface(Vector2f sample, Intersection *result, float *pdf) const {

}

Cone::Cone(const Json &json) : Shape(json) {
    radius = fetchOptional(json, "radius", 1.f);
    height = fetchOptional(json, "height", 1.f);
    phiMax = fetchOptional(json, "phi_max", 2 * PI);
    float tanTheta = radius / height;
    cosTheta = sqrt(1/(1+tanTheta * tanTheta));
    //theta = fetchOptional(json,)
    AABB localAABB = AABB(Point3f(-radius,-radius,0),Point3f(radius,radius,height));
    boundingBox = transform.toWorld(localAABB);
    boundingBox = AABB(Point3f(-100,-100,-100),Point3f(100,100,100));
}

REGISTER_CLASS(Cone, "cone")
