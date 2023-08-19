#include "Disk.h"
#include "ResourceLayer/Factory.h"
#include "CoreLayer/Math/Transform.h"
#include <iostream>
using namespace std;

bool Disk::rayIntersectShape(Ray &ray, int *primID, float *u, float *v) const {
    //* todo 完成光线与圆环的相交 填充primId,u,v.如果相交，更新光线的tFar
    //* 1.光线变换到局部空间
    //每个shape有自己的tranform
    Ray newRay=transform.inverseRay(ray);
    //* 2.判断局部光线的方向在z轴分量是否为0
    if(newRay.direction[2]==0) return false;
    //* 3.计算光线和平面交点
    float t=(0-newRay.origin[2])/(newRay.direction[2]);
    Point3f p=newRay.at(t);
    
    //* 4.检验交点是否在圆环内
    if(t<=newRay.tNear||t>=newRay.tFar) return false;
    //这里不确定如何访问三维坐标，下标012表示xyz?--yes
    float r=sqrt(p[0]*p[0]+p[1]*p[1]);
    if(r>radius||r<innerRadius) return false;
    //检验夹角
    float phi=atan2(p[1],p[0]);//atan2（y，x）求的是y/x的反正切，其返回值为[-pi,+pi]之间的一个数
    if(phi<0) phi+=2*PI; 
    if(phi>phiMax) return false;

    //* 5.更新ray的tFar,减少光线和其他物体的相交计算次数
    ray.tFar=t;//一定要更新原来的ray！！
    //* 6.填充
    * primID=0;
    * u =phi/phiMax;
    * v =(r-innerRadius)/(radius-innerRadius);
    return true;

    //* Write your code here.
    //return false;
}

void Disk::fillIntersection(float distance, int primID, float u, float v, Intersection *intersection) const {
    /// ----------------------------------------------------
    //* todo 填充圆环相交信息中的法线以及相交位置信息
    //* 1.法线可以先计算出局部空间的法线，然后变换到世界空间
    Vector3f normal(0.f,0.f,1.f);
    intersection->normal=transform.toWorld(normal);//法线已经归一化了
    //* 2.位置信息可以根据uv计算出，同样需要变换
    float phi=phiMax*u;
    float r=v*(radius-innerRadius)+innerRadius;
    float y=r/(sqrt(1/(tan(phi)*tan(phi))+1));
    if(phi>PI)//三四象限
    {
        y=-y;
    }
    float x=y/tan(phi);

    //printf("\nbefore: %lf,%lf\n",x,y);
    intersection->position=transform.toWorld(Point3f(x,y,0.f));
    //printf("after: %lf,%lf\n\n",intersection->position[0],intersection->position[1]);
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

Disk::Disk(const Json &json) : Shape(json) {
//    normal = transform.toWorld(Vector3f(0,0,1));
//    origin = transform.toWorld(Point3f(0,0,0));
//    auto
//    //radius认为是三个方向的上的scale平均
//    vecmat::vec4f v(1,1,1,0);
//    auto radiusVec = transform.scale * v;
//    radiusVec/=radiusVec[3];
//    radius = (radiusVec[0]+radiusVec[1]+radiusVec[2])/3;
     radius = fetchOptional(json,"radius",1.f);
     innerRadius = fetchOptional(json,"inner_radius",0.f);
     phiMax = fetchOptional(json,"phi_max",2 * PI);
     AABB local(Point3f(-radius,-radius,0),Point3f(radius,radius,0));
     boundingBox = transform.toWorld(local);
}

void Disk::uniformSampleOnSurface(Vector2f sample, Intersection *result, float *pdf) const {
        //采样光源 暂时不用实现
}
REGISTER_CLASS(Disk, "disk")

