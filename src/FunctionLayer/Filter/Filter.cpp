# include "Filter.h"


// 中值滤波
void  Filter::MedianFilter(const std::shared_ptr<Image>& src, std::shared_ptr<Image>& dst,double size)
{
    //边缘像素默认填充0
    //std::shared_ptr<Image> dst = std::shared_ptr<Image>(new Image(*src));
    int start = size / 2;
    for (int i = start; i < dst->size[1] - start; i++)
    {
        for (int j = start; j < dst->size[0] - start; j++)
        {
            // 收集邻域内的像素值
            std::vector<Vector3f> model;
            for (int m = i - start; m <= i + start; m++)
            {
                for (int n = j - start; n <= j + start; n++)
                {
                    model.push_back(src->getValue(Vector2i(n, m)));
                }
            }
            // 对邻域内的像素值进行排序
           std::sort(model.begin(), model.end(), [](const Vector3f& a, const Vector3f& b) {
            return a.length() < b.length();
            });

            // 取中值作为输出图像中的像素值
            dst->setValue(Vector2i(j, i), Vector3f(model[size * size / 2]));
        }
    }
}

// 均值滤波
void Filter::MeanFilter(const std::shared_ptr<Image>& src,std::shared_ptr<Image>& dst, double size)
{
    //std::shared_ptr<Image> dst = std::make_shared<Image>(*src);
    int start = size / 2;
    for (int i = start; i < dst->size[1] - start; i++)
    {
        for (int j = start; j < dst->size[0] - start; j++)
        {
            Vector3f sum(0.0f, 0.0f, 0.0f);
            for (int m = i - start; m <= i + start; m++)
            {
                for (int n = j - start; n <= j + start; n++)
                {
                    Vector3f value = src->getValue(Vector2i(n, m));
                    sum += value;
                }
            }
            Vector3f average = sum / (size * size);
            dst->setValue(Vector2i(j, i), average);
        }
    }
    
}

// 高斯滤波
void Filter::GaussianFilter(const std::shared_ptr<Image>& src, std::shared_ptr<Image>& dst,double size, double sigma)
{
    std::vector<std::vector<double>> gaussianTemplate = GaussianTemplate(size,sigma);
    int start = size / 2;
    for (int i = start; i < dst->size[1] - start; i++)
    {
        for (int j = start; j < dst->size[0] - start; j++)
        {
            Vector3f sum(0.0f, 0.0f, 0.0f);
            for (int m = i - start; m <= i + start; m++)
            {
                for (int n = j - start; n <= j + start; n++)
                {
                    Vector3f value = src->getValue(Vector2i(n, m));
                    float weight = gaussianTemplate[m - i + start][n - j + start];
                    sum += value * weight;
                }
            }
            dst->setValue(Vector2i(j, i), sum);
        }
    }
   
}

//高斯掩膜的求解与位置(x,y)无关，因为在计算过程中x,y被抵消掉了，因此只要计算一次就可以了
//需要size,sigma
//同时也是双边滤波的空间域模版
std::vector<std::vector<double>> Filter::GaussianTemplate(double size, double sigma)
{
    std::vector<std::vector<double>> temp;
    double base = 1.0 / 2.0 / PI / sigma / sigma;
    for(int i = 0; i < size; i++)
    {
        std::vector<double> vec;
        for(int j = 0; j < size; j++)
        {
            double a = (pow(i - size/2, 2) + pow(j - size/2, 2)) / 2.0 / sigma / sigma;
            double b = base * exp(-a);
            vec.push_back(b);
        }
        temp.push_back(vec);
    }
    return temp;
}


// 双边滤波,去噪保边
void Filter::BilateralFilter(const std::shared_ptr<Image>& src, std::shared_ptr<Image>& dst, int size, float sigmaD, float sigmaR)
{
    std::vector<std::vector<double>> spatialTemplate = GaussianTemplate(size, sigmaD);
    std::vector<float> rangeTemplate = RangeTemplate(256, sigmaR);

    int start = size / 2;

    for (int y = start; y < src->size[1] - start; ++y)
    {
        for (int x = start; x < src->size[0] - start; ++x)
        {
            Vector3f sum(0.0f, 0.0f, 0.0f);
            float weightSum = 0.0f;

            for (int i = y - start; i <= y + start; ++i)
            {
                for (int j = x - start; j <= x + start; ++j)
                {
                    Vector3f value = src->getValue(Vector2i(j, i));
                    float spatialWeight = spatialTemplate[i - y + start][j - x + start];
                    float rangeWeight = rangeTemplate[static_cast<int>(std::abs(value.length()))];
                    float weight = spatialWeight * rangeWeight;

                    sum += value * weight;
                    weightSum += weight;
                }
            }

            dst->setValue(Vector2i(x, y), sum / weightSum);
        }
    }
}


//值域模版
std::vector<float> Filter::RangeTemplate(int range,float sigmaR)
{
    std::vector<float> mask;
    float base = 1.0f / (2.0f * sigmaR * sigmaR);

    for (int i = 0; i < range; ++i)
    {
        float distance = i * i;
        float weight = std::exp(-distance / (2.0f * sigmaR * sigmaR));
        mask.push_back(weight);
    }

    return mask;
}