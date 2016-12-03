#include<iostream>
#include"device.h"
#include"resources.h"
#include"pipeline.h"
#include"command_buffer.h"

int main()
{
  std::vector<float> input {0.0f,1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,9.0f,10.0f,11.0f,12.0f,13.0f,14.0f,15.0f};
  std::vector<float> output(4);
  simpleVK::Device device;
  {
    simpleVK::Resources resources(device);
    {
      simpleVK::Pipeline pipeline(device,resources);
      {
        resources.writeInputBuffer(input);
        simpleVK::CommandBuffer commandBuffer(device,resources,pipeline);
        commandBuffer.dispach();
        resources.readOutputBuffer(output);
      }
      std::cout 
        << "OK" 
        << output[0] << ':'
        << output[1] << ':'
        << output[2] << ':'
        << output[3] << ':' << std::endl;
      abort();
    }
  }
}
