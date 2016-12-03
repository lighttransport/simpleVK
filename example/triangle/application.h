#include<vector>
#include<string>
#include<chrono>
#include<GLFW/glfw3.h>

namespace simpleVulkan
{
    class Application
    {
   private:
       uint32_t m_width = 400;
       uint32_t m_height = 400;
       GLFWwindow* m_window;
       std::chrono::milliseconds m_interval;
	   bool m_validate; // Enable Vulkan validation layer + debug report?

   public:
       Application();
       virtual ~Application();

       bool create(std::string windowName,uint32_t width,uint32_t height, bool validate = false);
       void destroy();
       bool run();
   protected:
       std::chrono::microseconds getInterval();
       void setInterval(std::chrono::milliseconds interval);
       uint32_t getWidth();
       void setSize(uint32_t width,uint32_t height);
       uint32_t getHeight();
       void setHeight(uint32_t height);
	   bool getValidateFlag();
	   void setValidateFlag(bool validate);
       void closeWindow();
   private:
       virtual bool initialize(
               const std::vector<const char*>& glfwExtensions,
               GLFWwindow* window) = 0;
       virtual void finalize() = 0;
       virtual bool render() = 0;
    };
}
