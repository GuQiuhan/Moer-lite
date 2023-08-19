#include "Octree.h"
#include <queue>
#include <vector>
#include <iostream>
struct Octree::OctreeNode {
    AABB boundingBox;
    OctreeNode* subNodes[8];//8个子节点的指针
    int primCount = -1;
    std::vector<int> primIdxBuffer;//叶节点存储的物体id,可能超过最大数量，当某一包围盒与当前节点的所有物体都相交时

    //构造函数
    OctreeNode(const AABB& box, const std::vector<int> &idxBuffer)//叶子结点
    {
      //填写存储的图元
      primIdxBuffer=idxBuffer;
      primCount=idxBuffer.size();//图元数量

      //填写包围框
      boundingBox=box;

      //填写子节点，叶子节点无子节点
      for(int j=0; j<8; ++j) subNodes[j]=nullptr;

    }

    OctreeNode(const AABB& box)//非叶子节点
    {
        boundingBox=box;
        primCount=-1;//非叶子节点下没有图元
    }


};


Octree::OctreeNode * Octree::recursiveBuild(const AABB &aabb,const std::vector<int>& primIdxBuffer,int depth) {
  //* todo 完成递归构建八叉树
  //* 构建方法请看实验手册
  //* 要注意的一种特殊是当节点的某个子包围盒和当前节点所有物体都相交，我们就不用细分了，当前节点作为叶子节点即可。
  if(primIdxBuffer.size()<ocLeafMaxSize ||depth>=maxDepth)//先不管深度？
  {   
      OctreeNode* node = new OctreeNode(aabb,primIdxBuffer);
      return node;
  }

  std::vector<AABB> subBoxes=aabb.getSubBoxs();//分成8个子盒子

  std::vector<std::vector<int>> subBuffers(8);//8个盒子分别对应的子节点
  
  for(int i = 0; i<8 ;i++)
  {
    
      for(auto index : primIdxBuffer)
      {
        if(shapes[index]->getAABB().Overlap(subBoxes[i]))
        {          
          subBuffers[i].push_back(index);
        }
      }
      
      if(subBuffers[i].size()==primIdxBuffer.size())//子包围盒和所有物体都相交
      {
        OctreeNode* node = new OctreeNode(subBoxes[i],primIdxBuffer);
        return node;
        
      }
  }
 

  OctreeNode* node = new OctreeNode(aabb);//创建一个非叶子节点
  for(int i = 0;i<8;i++)
  {   
    node->subNodes[i]=recursiveBuild(subBoxes[i],subBuffers[i],depth+1);
  }

  return node;
}


void Octree::build() {
  //* 首先计算整个场景的范围
  for (const auto & shape : shapes) {
    //* 自行实现的加速结构请务必对每个shape调用该方法，以保证TriangleMesh构建内部加速结构
    //* 由于使用embree时，TriangleMesh::getAABB不会被调用，因此出于性能考虑我们不在TriangleMesh
    //* 的构造阶段计算其AABB，因此当我们将TriangleMesh的AABB计算放在TriangleMesh::initInternalAcceleration中
    //* 所以请确保在调用TriangleMesh::getAABB之前先调用TriangleMesh::initInternalAcceleration
    shape->initInternalAcceleration();

    boundingBox.Expand(shape->getAABB());
  }

  //* 构建八叉树
  std::vector<int> primIdxBuffer(shapes.size());//所有的shape
  std::iota(primIdxBuffer.begin(), primIdxBuffer.end(), 0);
  
  root = recursiveBuild(boundingBox, primIdxBuffer,0);//从根节点开始递归构建树

}

bool Octree::rayIntersect(Ray &ray, int *geomID, int *primID,float *u, float *v) const {
  //*todo 完成八叉树求交
 
  return this->recursiveRayIntersect(root,ray,geomID,primID,u,v);
  //return false;
}



bool Octree::recursiveRayIntersect(OctreeNode* node, Ray &ray, int *geomID, int *primID, float *u, float *v) const
{
  if(node==nullptr) return false;
  if(!node->boundingBox.RayIntersect(ray,nullptr,nullptr)) return false;

  //相交
  //对于非叶子节点
  if(node->primCount==-1)
  {                 
    bool flag=false;
    for(auto subNode:node->subNodes)
    {
      //if( subNode==nullptr) continue;
      flag|=recursiveRayIntersect(subNode,ray,geomID,primID,u,v);
      //if(flag==true) break;//不要break，要找最终的相交
    }
    return flag;
  }

  else if(node->primCount!=0)//叶子节点
  {
    bool flag=false;
    for(auto idx:node->primIdxBuffer)
    {
      bool re=shapes[idx]->rayIntersectShape(ray,primID,u,v);
      flag|=re;
      if(re==true)
      { 
        *geomID=shapes[idx]->geometryID;
        //break;
      }
    }
    return flag;
  }

  

  return false;

}