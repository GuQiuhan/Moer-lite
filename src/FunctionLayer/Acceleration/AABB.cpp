#include "AABB.h"
#include <iostream>

Point3f minP(const Point3f &p1, const Point3f &p2) {
  return Point3f{std::min(p1[0], p2[0]), std::min(p1[1], p2[1]),
                 std::min(p1[2], p2[2])};
}

Point3f maxP(const Point3f &p1, const Point3f &p2) {
  return Point3f{std::max(p1[0], p2[0]), std::max(p1[1], p2[1]),
                 std::max(p1[2], p2[2])};
}

AABB AABB::Union(const AABB &other) const {
  Point3f min = minP(other.pMin, pMin), max = maxP(other.pMax, pMax);
  return AABB{min, max};
}

void AABB::Expand(const AABB &other) {
  pMin = minP(pMin, other.pMin);
  pMax = maxP(pMax, other.pMax);
}

AABB AABB::Union(const Point3f &other) const {
  Point3f min = minP(other, pMin), max = maxP(other, pMax);
  return AABB{min, max};
}

void AABB::Expand(const Point3f &other) {
  pMin = minP(pMin, other);
  pMax = maxP(pMax, other);
}

bool AABB::Overlap(const AABB &other) const {
  for (int dim = 0; dim < 3; ++dim) {
    if (pMin[dim] > other.pMax[dim] || pMax[dim] < other.pMin[dim]) { //加个等于
      return false;
    }
  }
  return true;
}

bool AABB::RayIntersect(const Ray &ray, float *tMin, float *tMax) const {
  float tNear = ray.tNear, tFar = ray.tFar;
  for (int i = 0; i < 3; ++i) {
    float invDir = 1.f / ray.direction[i];
    float t0 = (pMin[i] - ray.origin[i]) * invDir,
          t1 = (pMax[i] - ray.origin[i]) * invDir;
    if (t0 > t1)
      std::swap(t0, t1);
    tNear = std::max(tNear, t0);
    tFar = std::min(tFar, t1);

    if (tNear > tFar)
      return false;
  }
  if (tMin)
    *tMin = tNear;
  if (tMax)
    *tMax = tFar;
  return true;
}

Point3f AABB::Center() const {
  return Point3f{(pMin[0] + pMax[0]) * .5f, (pMin[1] + pMax[1]) * .5f,
                 (pMin[2] + pMax[2]) * .5f};
}

//划分空间得到子包围盒
std::vector<AABB> AABB::getSubBoxs() const
  {
    std::vector<AABB> boxs;
    //boxs.resize(8);
    Point3f A(pMin);
    Point3f B(pMax);
    float dx=(B[0]-A[0])/2;
    float dy=(B[1]-A[1])/2;
    float dz=(B[2]-A[2])/2;
    
    boxs.push_back(AABB(Point3f(A[0],A[1]+dy,A[2]),Point3f(A[0]+dx,A[1]+2*dy,A[2]+dz)));//底层后左
    boxs.push_back(AABB(Point3f(A[0]+dx,A[1]+dy,A[2]),Point3f(A[0]+2*dx,A[1]+2*dy,A[2]+dz)));//底层后右

    boxs.push_back(AABB(Point3f(A[0],A[1],A[2]+dz),Point3f(A[0]+dx,A[1]+dy,A[2]+2*dz)));//上层前左
    boxs.push_back(AABB(Point3f(A[0]+dx,A[1],A[2]+dz),Point3f(A[0]+2*dx,A[1]+dy,A[2]+2*dz)));//上层前右
    boxs.push_back(AABB(Point3f(A[0],A[1]+dy,A[2]+dz),Point3f(A[0]+dx,A[1]+2*dy,A[2]+2*dz)));//上层后左
    boxs.push_back(AABB(Point3f(A[0]+dx,A[1]+dy,A[2]+dz),Point3f(A[0]+2*dx,A[1]+2*dy,A[2]+2*dz)));//上层后右

    boxs.push_back(AABB(A,Point3f(A[0]+dx,A[1]+dy,A[2]+dz)));//底层前左
    boxs.push_back(AABB(Point3f(A[0]+dx,A[1],A[2]),Point3f(A[0]+2*dx,A[1]+dy,A[2]+dz)));//底层前右
   
   
    return boxs;


  }