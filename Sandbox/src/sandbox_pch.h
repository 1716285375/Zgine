#pragma once

// Sandbox预编译头文件
// 包含Sandbox应用程序常用的头文件

// 标准库
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <filesystem>


// 数学库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Zgine引擎
#include "Zgine.h"

// ImGui
#include <imgui.h>

// 日志
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// 平台特定
#ifdef ZG_PLATFORM_WINDOWS
    #include <windows.h>
#endif
