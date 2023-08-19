#pragma once
#include "Acceleration.h"
class Octree : public Acceleration {
public:
  Octree() = default;
  void build() override;
  bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                            float *v) const override;

  
protected:
  struct OctreeNode;
  OctreeNode * root;
  static constexpr int maxDepth = 10;
  static constexpr int ocLeafMaxSize = 64;

  OctreeNode * recursiveBuild(const AABB &aabb,const std::vector<int>& primIdxBuffer,int depth);
  bool recursiveRayIntersect(OctreeNode* node,Ray &ray, int *geomID, int *primID,float *u, float *v) const ;

  
  
};
