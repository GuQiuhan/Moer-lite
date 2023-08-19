#pragma once
#include "Acceleration.h"
class BVH : public Acceleration{
public:
    BVH() = default;
    void build() override;
    bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u, float *v) const override;
protected:
    static constexpr int bvhLeafMaxSize = 64;
    static constexpr int maxDepth = 10;
    struct BVHNode;
    BVHNode * root;

    //第二种递归构建方法
    BVHNode * recursiveBuild(const AABB& aabb,std::vector<int>& primIdxBuffer , int l, int r,std::vector<std::shared_ptr< Shape>> & orderedShapes,int depth);
    
    //第一种递归构建方法，以x轴分割，不正确！！
    BVHNode * recursiveBuild(const AABB& aabb,std::vector<int>& primIdxBuffer , std::vector<std::shared_ptr< Shape>> & orderedShapes,int depth);
    
    bool recursiveRayIntersect(BVHNode* node,Ray &ray, int *geomID, int *primID,float *u, float *v) const ;

  
    //实现排序
    void QuickSort(std::vector<int>& primIdxBuffer, int low, int high,int split);
    int Paritition1(std::vector<int>& primIdxBuffer, int low, int high,int split);

};


