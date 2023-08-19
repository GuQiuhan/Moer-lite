#include "BVH.h"

//实现排序  
void BVH::QuickSort(std::vector<int>& primIdxBuffer, int low, int high,int split) //快排母函数
{
    if (low < high) {
        int pivot = Paritition1(primIdxBuffer, low, high,split);
        QuickSort(primIdxBuffer, low, pivot - 1,split);
        QuickSort(primIdxBuffer, pivot + 1, high,split);
    }
}

int BVH::Paritition1(std::vector<int>& primIdxBuffer, int low, int high,int split)
{
    int pivot = primIdxBuffer[low];
    while (low < high) 
    {
        while (low < high && shapes[primIdxBuffer[high]]->getAABB().Center()[split] >= shapes[pivot]->getAABB().Center()[split])
        {
            --high;
        }
        primIdxBuffer[low] = primIdxBuffer[high];
        while (low < high && shapes[primIdxBuffer[low]]->getAABB().Center()[split] <= shapes[pivot]->getAABB().Center()[split]) {
            ++low;
        }
        primIdxBuffer[high] = primIdxBuffer[low];
   }
   primIdxBuffer[low] = pivot;
   return low;
}


struct BVH::BVHNode
{
    //* todo BVH节点结构设计
    BVHNode * left;
    BVHNode * right;//左右节点
    AABB boundingBox;
    
    //叶子节点存节点索引
    int firstShapeOffset;//第一个物体的索引
    int nShape = 0;//一共有多少物体

    int splitAxis;//非叶子节点的分割轴，求交部分用

    BVHNode(const AABB& aabb)
    {
        boundingBox=aabb;
        left=nullptr;
        right=nullptr;
        nShape=-1;//默认为非叶子节点
    }
     
};

void BVH::build()
{
    //AABB sceneBox;
    for (const auto &shape : shapes)
    {
        //* 自行实现的加速结构请务必对每个shape调用该方法，以保证TriangleMesh构建内部加速结构
        //* 由于使用embree时，TriangleMesh::getAABB不会被调用，因此出于性能考虑我们不在TriangleMesh
        //* 的构造阶段计算其AABB，因此当我们将TriangleMesh的AABB计算放在TriangleMesh::initInternalAcceleration中
        //* 所以请确保在调用TriangleMesh::getAABB之前先调用TriangleMesh::initInternalAcceleration
        shape->initInternalAcceleration();
        boundingBox.Expand(shape->getAABB());
    }
    //* todo 完成BVH构建
  std::vector<int> primIdxBuffer(shapes.size());//所有的shape
  std::iota(primIdxBuffer.begin(), primIdxBuffer.end(), 0);
  
  std::vector<std::shared_ptr< Shape>>  orderedShapes;//存储构建好的shapes顺序
  orderedShapes.clear();

  root = recursiveBuild(boundingBox, primIdxBuffer,0,primIdxBuffer.size()-1,orderedShapes,0);//从根节点开始递归构建树

  //root = recursiveBuild(boundingBox, primIdxBuffer,orderedShapes,0);//从根节点开始递归构建树
  shapes=orderedShapes;//重制shapes

}

//正确递归方式
//[l,r]范围内
BVH::BVHNode * BVH::recursiveBuild(const AABB& aabb,std::vector<int>& primIdxBuffer , int l, int r,std::vector<std::shared_ptr< Shape>> & orderedShapes,int depth) 
{
    if ( r-l+1 <= bvhLeafMaxSize ||depth>maxDepth) 
    {
        BVHNode* node=new BVHNode(aabb);
        node->firstShapeOffset = orderedShapes.size();
        node->nShape =  r-l+1;
        for ( int i = l ; i <= r ; ++ i ) 
        {
            orderedShapes.push_back(shapes[primIdxBuffer[i]]);
        }

        return node;
    }

    //计算图元**质心**的边界!!选择分割的坐标轴
    AABB b;
    for (int i = l; i <= r; ++i)
    b.Expand( shapes[primIdxBuffer[i]]->getAABB().Center());
    Point3f A(b.pMin);
    Point3f B(b.pMax);
    float dx=(B[0]-A[0]);
    float dy=(B[1]-A[1]);
    float dz=(B[2]-A[2]);
    AABB Box1;//负半边盒子
    AABB Box2;//正半边盒子

    BVHNode* node = new BVHNode(aabb);//创建一个非叶子节点 

    if(dx>=dy&&dx>=dz) node->splitAxis=0;//按照x轴划分
    else if(dy>=dx&&dy>=dz) node->splitAxis=1;//按照y轴划分
    else node->splitAxis=2;//按照z轴划分

    if(A[node->splitAxis]==B[node->splitAxis]) //质心相等
    {
        printf("here/n");
        BVHNode* node=new BVHNode(aabb);
        node->firstShapeOffset = orderedShapes.size();
        node->nShape =  r-l+1;
        for ( int i = l ; i <= r ; ++ i ) 
        {
            orderedShapes.push_back(shapes[primIdxBuffer[i]]);
        }

        return node;
    }

    QuickSort(primIdxBuffer,l,r,node->splitAxis);//排序

    int mid=(r+l)/2;
    for(int i=l; i<=mid; ++i)
    {
        Box1.Expand(shapes[primIdxBuffer[i]]->getAABB());
    }
    for(int i=mid+1; i<=r; ++i)
    {
        Box2.Expand(shapes[primIdxBuffer[i]]->getAABB());
    }

    node->left=recursiveBuild(Box1,primIdxBuffer,l,mid,orderedShapes,depth+1);
    node->right=recursiveBuild(Box2,primIdxBuffer,mid+1,r,orderedShapes,depth+1);
    

    return node;
    
}


//错误递归方式
BVH::BVHNode * BVH::recursiveBuild(const AABB& aabb,std::vector<int>& primIdxBuffer , std::vector<std::shared_ptr< Shape>> & orderedShapes,int depth) 
{
    if ( primIdxBuffer.size() <= bvhLeafMaxSize ||depth>maxDepth) 
    {
        BVHNode* node=new BVHNode(aabb);
        node->firstShapeOffset = orderedShapes.size();
        node->nShape =  primIdxBuffer.size();
        for ( int i = 0; i < primIdxBuffer.size() ; ++ i ) 
        {
            orderedShapes.push_back(shapes[primIdxBuffer[i]]);
        }

        return node;
    }

    Point3f A(aabb.pMin);
    Point3f B(aabb.pMax);
    float dx=(B[0]-A[0])/2;
    float xme=dx+A[0];

    AABB leftBox;//左半边盒子
    AABB rightBox;//右半边盒子

    BVHNode* node = new BVHNode(aabb);//创建一个非叶子节点 
    node->splitAxis=0;

    std::vector<std::vector<int>> subBuffers(2);//8个盒子分别对应的子节点
    
    for(auto index : primIdxBuffer)
      {
        if(shapes[index]->getAABB().Center()[0]<xme)
        {          
          subBuffers[0].push_back(index);
          leftBox.Expand(shapes[index]->getAABB());
        }
        else
        { subBuffers[1].push_back(index);
            rightBox.Expand(shapes[index]->getAABB());
        }

      }

    node->left=recursiveBuild(leftBox,subBuffers[0],orderedShapes,depth+1);
    node->right=recursiveBuild(rightBox,subBuffers[1],orderedShapes,depth+1);

    return node;
    
}





bool BVH::rayIntersect(Ray &ray, int *geomID, int *primID, float *u, float *v) const {
    //* todo 完成BVH求交

    return this->recursiveRayIntersect(root,ray,geomID,primID,u,v);
    
}

bool BVH::recursiveRayIntersect(BVHNode *node, Ray &ray, int *geomID, int *primID, float *u, float *v) const
{
    if(node==nullptr) return false;
    if(!node->boundingBox.RayIntersect(ray,nullptr,nullptr)) return false;

    //相交
    //对于非叶子节点
    if(node->nShape==-1)
    {                 
         if(ray.direction[node->splitAxis]>0)//先调用左子节点
         {
             if(recursiveRayIntersect(node->left,ray,geomID,primID,u,v)) return true;
             else return recursiveRayIntersect(node->right,ray,geomID,primID,u,v);
         }

         else//先调用右子节点
         {
             if(recursiveRayIntersect(node->right,ray,geomID,primID,u,v)) return true;
             else return recursiveRayIntersect(node->left,ray,geomID,primID,u,v);
         }
        //return recursiveRayIntersect(node->left,ray,geomID,primID,u,v) || recursiveRayIntersect(node->right,ray,geomID,primID,u,v);
        
    }

    else if(node->nShape!=0)//叶子节点
    {
        bool flag=false;
        for(int i=0; i<node->nShape; ++i)
        {
            bool re=shapes[node->firstShapeOffset+i]->rayIntersectShape(ray,primID,u,v);
            flag|=re;
            if(re==true)
            { 
                *geomID=shapes[node->firstShapeOffset+i]->geometryID;
                //break;
            }
        }
        return flag;
    }

    

    return false;

}

