#include<iostream>
#include<chrono>
#include<memory>
#include"window.h"
#include"devices_manager.h"
#include"resources.h"

int main()
{
#ifdef USE_VALIDATION_LAYER
	std::cout << "isDebugging" << std::endl;
#endif
	auto devices = std::make_shared<simpleVK::multiGPU::DevicesManager>();
	auto window = std::make_shared<simpleVK::multiGPU::Window>(
			devices,
			"MultiGPU",
			400,
			400,
			std::chrono::milliseconds(10));
	auto resources = std::make_shared<simpleVK::multiGPU::Resources>(devices,window);
	window->run([](){});
}
