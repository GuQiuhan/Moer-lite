#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Camera/Pinhole.h>
#include <FunctionLayer/Integrator/Integrator.h>
#include <FunctionLayer/Sampler/Sampler.h>
#include <FunctionLayer/Filter/Filter.h>
#include <FunctionLayer/Noise/Noise.h>
#include <FunctionLayer/Scene/Scene.h>
#include <FunctionLayer/Texture/Mipmap.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/FileUtil.h>
#include <ResourceLayer/Image.h>
#include <ResourceLayer/JsonUtil.h>
#include <chrono>
#include <fstream>
#include <regex>
#include <stdio.h>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

inline void printProgress(float percentage) {
  int val = (int)(percentage * 100);
  int lpad = (int)(percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);
}

int main(int argc, char **argv) {
  const std::string sceneDir = std::string(argv[1]); //从命令行参数中获取场景目录的路径
  FileUtil::setWorkingDirectory(sceneDir); //并设置当前工作目录为场景目录
  std::string sceneJsonPath = FileUtil::getFullPath("scene.json"); //构建场景描述文件的完整路径
  std::ifstream fstm(sceneJsonPath); //打开文件，变量名为fstm
  Json json = Json::parse(fstm); //解析场景描述文件为JSON格式
  //根据JSON中的信息构建相机、场景、积分器和采样器对象
  auto camera = Factory::construct_class<Camera>(json["camera"]);
  auto scene = std::make_shared<Scene>(json["scene"]);
  auto integrator = Factory::construct_class<Integrator>(json["integrator"]);
  auto sampler = Factory::construct_class<Sampler>(json["sampler"]);

  //获取每个像素的采样数和图像的宽度和高度
  int spp = sampler->xSamples * sampler->ySamples;
  int width = camera->film->size[0], height = camera->film->size[1];

  auto start = std::chrono::system_clock::now();

  //循环遍历每个像素，进行渲染计算
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Vector2f NDC{(float)x / width, (float)y / height}; //计算当前像素的归一化设备坐标(Normalized Device Coordinates)
      Spectrum li(.0f); //创建一个初始颜色贡献
      for (int i = 0; i < spp; ++i) {
        Ray ray = camera->sampleRayDifferentials(
            CameraSample{sampler->next2D()}, NDC); //根据生成的采样点和NDC坐标生成一条光线
        li += integrator->li(ray, *scene, sampler); //调用积分器，传入光线、场景scene和采样器sampler，计算得到该光线的颜色贡献
      }
      camera->film->deposit({x, y}, li / spp); //除以采样次数spp，得到平均的颜色贡献

      int finished = x + y * width;
      if (finished % 5 == 0) {
        printProgress((float)finished / (height * width));
      }
    }
  }
  printProgress(1.f);

  auto end = std::chrono::system_clock::now();

  printf("\nRendering costs %.2fs\n",
         (std::chrono::duration_cast<std::chrono::milliseconds>(end - start))
                 .count() /
             1000.f);


  //* 目前支持输出为png/hdr两种格式
  std::string outputName =
      fetchRequired<std::string>(json["output"], "filename");
  if (std::regex_match(outputName, std::regex("(.*)(\\.png)"))) {
    camera->film->savePNG(outputName.c_str());
  } else if (std::regex_match(outputName, std::regex("(.*)(\\.hdr)"))) {
    camera->film->saveHDR(outputName.c_str());
    camera->film->savePFM("test0.pfm");
  } else {
    std::cout << "Only support output as PNG/HDR\n";
    return 0;
  }

  //需要加噪声/滤波
  if (argc == 4 ) {
    // 命令格式为“./Moer <样例路径> <加噪声> <去噪>”
    const std::string noise = std::string(argv[2]); // 获取第三个参数的值，噪声类型
    const std::string denoise = std::string(argv[3]); // 获取第四个参数的值，去噪类型
    //存储加噪声的图片
    std::string outputName1 = fetchRequired<std::string>(json["output"], "filename");
    std::size_t extensionPos1 = outputName.find_last_of(".");
    //存储去噪的图片
    std::string outputName2 = fetchRequired<std::string>(json["output"], "filename");
    std::size_t extensionPos2 = outputName.find_last_of(".");
    
    //加噪声
    printf("\n********Begin Add Noise********\n");
    if(noise=="add_salt_noise") {
      camera->film->addNoise(1);
      if (extensionPos1 != std::string::npos) {
        outputName1.insert(extensionPos1, "_add_salt_noise");
      }
    }
    else if(noise=="add_g_noise") {
      camera->film->addNoise(2);
      if (extensionPos1 != std::string::npos) {
        outputName1.insert(extensionPos1, "_add_g_noise");
      }
    }
    else if(noise=="none") {
        
    }
    else {
      std::cout<<"Only support add_salt_noise, add_g_noise, none\n";
    }
 
    if (std::regex_match(outputName1, std::regex("(.*)(\\.png)"))) {
      camera->film->savePNG(outputName1.c_str());
    } else if (std::regex_match(outputName1, std::regex("(.*)(\\.hdr)"))) {
      camera->film->saveHDR(outputName1.c_str());
    } else {
      std::cout << "Only support output as PNG/HDR\n";
    }

    std::cout<< "Save noise image to "<<outputName1<<std::endl;

    //去噪
    auto s = std::chrono::system_clock::now();
    auto e = std::chrono::system_clock::now();;
    printf("\n********Begin Denoise********\n");
    if(denoise=="median_denoise") {
      camera->film->filter(1);
      e = std::chrono::system_clock::now();
      if (extensionPos2 != std::string::npos) {
        outputName2.insert(extensionPos2, "_median_denoise");
      }
    }
    else if(denoise=="mean_denoise") {
      camera->film->filter(2);
      e = std::chrono::system_clock::now();
      if (extensionPos2 != std::string::npos) {
        outputName2.insert(extensionPos2, "_mean_denoise");
      }
    }
    else if(denoise=="g_denoise") {
      camera->film->filter(3);
      e = std::chrono::system_clock::now();
      if (extensionPos2 != std::string::npos) {
        outputName2.insert(extensionPos2, "_g_denoise");
      }
    }
    else if(denoise=="bi_denoise") {
      camera->film->filter(4);
      e = std::chrono::system_clock::now();
      if (extensionPos2 != std::string::npos) {
        outputName2.insert(extensionPos2, "_bi_denoise");
      }
    }
    else if(denoise=="intel_denoise") {
        camera->film->savePFM("tmp.pfm");
        const char* command = "./oidnDenoise -hdr tmp.pfm -o re.pfm"; 
        // 使用system函数执行Shell命令
        system(command); 
        e = std::chrono::system_clock::now();
         std::cout<< "Save denoised image to re.pfm"<<std::endl;
        printf("\nIntel Denoising costs %.2fs\n",(std::chrono::duration_cast<std::chrono::milliseconds>(e - s)) .count() / 1000.f);
        return 0; 
    }
    else if(denoise=="denoise") {  //特别增加，用于测试
        //双边+降噪库
        camera->film->filter(4);
        e = std::chrono::system_clock::now();
        if (extensionPos2 != std::string::npos) {
          outputName2.insert(extensionPos2, "_bi_denoise");
        if (std::regex_match(outputName2, std::regex("(.*)(\\.png)"))) {
          camera->film->save_dePNG(outputName2.c_str());
        } else if (std::regex_match(outputName2, std::regex("(.*)(\\.hdr)"))) {
          camera->film->save_deHDR(outputName2.c_str());
        } else {
          std::cout << "Only support output as PNG/HDR\n";
        }
            
        camera->film->savePFM("tmp.pfm");
        const char* command = "./oidnDenoise -hdr tmp.pfm -o re.pfm";
        // 使用system函数执行Shell命令
        system(command);
        e = std::chrono::system_clock::now();
         std::cout<< "Save denoised image to re.pfm"<<std::endl;
        printf("\nIntel Denoising costs %.2fs\n",(std::chrono::duration_cast<std::chrono::milliseconds>(e - s)) .count() / 1000.f);
        return 0;
    }
    }
    else if(denoise=="none") {
       printf("\nIntel Denoising costs %.2fs\n",(std::chrono::duration_cast<std::chrono::milliseconds>(e - s)) .count() / 1000.f);
       return 0;
    }
    else {
      std::cout<<"Only support median_denoise, mean_denoise, g_denoise, bi_denoise, intel_denoise, none\n";
    }

  
    if (std::regex_match(outputName2, std::regex("(.*)(\\.png)"))) {
      camera->film->save_dePNG(outputName2.c_str());
    } else if (std::regex_match(outputName2, std::regex("(.*)(\\.hdr)"))) {
      camera->film->save_deHDR(outputName2.c_str());
    } else {
      std::cout << "Only support output as PNG/HDR\n";
    }

    std::cout<< "Save denoised image to "<<outputName2<<std::endl;
    // 计算去噪用时
    printf("\nIntel Denoising costs %.2fs\n",(std::chrono::duration_cast<std::chrono::milliseconds>(e - s)) .count() / 1000.f);
        
  }

  
}
