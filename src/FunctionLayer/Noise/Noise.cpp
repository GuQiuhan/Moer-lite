#include "Noise.h"

// 添加椒盐噪声
void Noise::AddSaltNoise(const std::shared_ptr<Image>& img, int n) {
    int width = img->size[0];
    int height = img->size[1];

    for (int k = 0; k < n; k++) {
        int x = rand() % width;
        int y = rand() % height;

        Vector3f whitePixel(255, 255, 255); // White color

        img->setValue(Vector2i(x, y), whitePixel);
    }

    for (int k = 0; k < n; k++) {
        int x = rand() % width;
        int y = rand() % height;

        Vector3f blackPixel(0, 0, 0); // Black color

        img->setValue(Vector2i(x, y), blackPixel);
    }
}

// 添加高斯噪声
void Noise::AddGaussianNoise(const std::shared_ptr<Image>& img, double mu, double sigma) {
 
    int width = img->size[0];
    int height = img->size[1];
    int channels = Image::channels;

    int RandomArray[width+1][height+1];
    for (int i = 0; i < width; i++) {
		for (int j = 0; j <height; j++) {
			RandomArray[i][j] = (rand() % (255 - 0 + 1)) + 0;
			//要取得[a,b)的随机整数，使用(rand() % (b-a))+ a;   			
			//要取得[a,b]的随机整数，使用(rand() % (b-a+1))+ a;   			
			//要取得(a,b]的随机整数，使用(rand() % (b-a))+ a + 1;   		
		}
	}
	//计算要添加噪声的数据范围小于20%
	int down1 = 0, up1 = 255 * 1 / 20;//0-0.05

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (RandomArray[i][j]<up1&&RandomArray[i][j]>down1){
                Vector2i xy(i, j);
                
                if (channels == 1) {  //只有一个通道，是灰度图
                    // 构建高斯噪声
                    double u1, u2;
                    do {
                        u1 = rand() * (1.0 / RAND_MAX);
                        u2 = rand() * (1.0 / RAND_MAX);
                    } while (u1 <= std::numeric_limits<double>::min()); // u1不能为0

                    double z = sigma * sqrt(-2.0 * log(u1)) * cos(2 * PI * u2) + mu;

                    Vector3f pixelVal = img->getValue(xy);
                    int val = static_cast<int>(pixelVal[0] + z * 32);
                    val = (val < 0) ? 0 : val;
                    val = (val > 255) ? 255 : val;

                    Vector3f newPixelVal(val, val, val);
                    img->setValue(xy, newPixelVal);
                } else { //多个通道，是彩色图
        

                    for (int k = 0; k < channels; k++) {
                        // 构建高斯噪声
                        //产生随机数
                        double u1, u2;
                        do {
                            u1 = rand() * (1.0 / RAND_MAX);
                            u2 = rand() * (1.0 / RAND_MAX);
                        } while (u1 <= std::numeric_limits<double>::min()); // u1不能为0

                        //利用 Box-Muller 公式将两个均匀分布的随机数转换为服从均值为 mu，标准差为 sigma 的高斯分布的随机数 z
                        double z = sigma * sqrt(-2.0 * log(u1)) * cos(2 * PI * u2) + mu;

                        Vector3f pixelVal = img->getValue(xy);
                        //这里有一个z的缩放系数用来调整随机数 z， 暂且设为除以50，可以动态调整
                        int val = static_cast<int>(pixelVal[k] + z*32);
                        val = (val < 0) ? 0 : val;
                        val = (val > 255) ? 255 : val;

                        pixelVal[k] = val;
                        img->setValue(xy, pixelVal);
                    }
                }
            }
        }
    }



    

}


