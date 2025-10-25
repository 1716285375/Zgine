#pragma once

// Standard Library Headers (most frequently used)
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>

// Third-party libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <miniaudio.h>

// Zgine Core Headers
#include "Zgine/Logging/Log.h"
#include "Zgine/Core/SmartPointers.h"

#ifdef ZG_PLATFORM_WINDOWS
	#include <Windows.h>
#endif