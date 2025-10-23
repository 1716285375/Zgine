# Zgine Engine API Documentation

## BatchRenderer2D

### Overview
High-performance 2D batch renderer with memory optimization and advanced features.

### Core Methods

#### Initialization
```cpp
static void Init();
static void Shutdown();
static bool IsInitialized();
```

#### Scene Management
```cpp
static void BeginScene(const OrthographicCamera& camera);
static void EndScene();
static void Flush();
```

#### Drawing Primitives

##### Quads
```cpp
// Basic quad with color
static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

// Textured quad
static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));

// Rotated quad
static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);

// Rotated textured quad
static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
```

##### Circles
```cpp
static void DrawCircle(const glm::vec2& position, float radius, const glm::vec4& color, int segments = 32);
static void DrawCircle(const glm::vec3& position, float radius, const glm::vec4& color, int segments = 32);
```

##### Lines
```cpp
static void DrawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color, float thickness = 0.02f);
static void DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color, float thickness = 0.02f);
```

##### Triangles
```cpp
static void DrawTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec4& color);
static void DrawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color);
```

##### Ellipses
```cpp
static void DrawEllipse(const glm::vec2& position, float radiusX, float radiusY, const glm::vec4& color, int segments = 32);
static void DrawEllipse(const glm::vec3& position, float radiusX, float radiusY, const glm::vec4& color, int segments = 32);
```

##### Arcs
```cpp
static void DrawArc(const glm::vec2& position, float radius, float startAngle, float endAngle, const glm::vec4& color, float thickness = 0.02f, int segments = 32);
static void DrawArc(const glm::vec3& position, float radius, float startAngle, float endAngle, const glm::vec4& color, float thickness = 0.02f, int segments = 32);
```

##### Gradients
```cpp
// Quad with gradient colors
static void DrawQuadGradient(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colorTopLeft, const glm::vec4& colorTopRight, const glm::vec4& colorBottomLeft, const glm::vec4& colorBottomRight);
static void DrawQuadGradient(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colorTopLeft, const glm::vec4& colorTopRight, const glm::vec4& colorBottomLeft, const glm::vec4& colorBottomRight);

// Rotated quad with gradient colors
static void DrawRotatedQuadGradient(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& colorTopLeft, const glm::vec4& colorTopRight, const glm::vec4& colorBottomLeft, const glm::vec4& colorBottomRight);
static void DrawRotatedQuadGradient(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colorTopLeft, const glm::vec4& colorTopRight, const glm::vec4& colorBottomLeft, const glm::vec4& colorBottomRight);
```

### Performance Optimization

#### Configuration
```cpp
// Set maximum number of quads per batch
static void SetMaxQuads(uint32_t maxQuads);

// Enable/disable ring buffer usage
static void SetUseRingBuffer(bool useRingBuffer);

// Enable/disable texture caching
static void SetEnableTextureCaching(bool enableCaching);

// Optimize renderer for specific scene requirements
static void OptimizeForScene(uint32_t expectedObjectCount, bool useRingBuffer = true, bool enableTextureCaching = true);
```

#### Statistics
```cpp
// Get current render statistics
static RenderStats GetStats();

// Reset statistics
static void ResetStats();
```

### Data Structures

#### QuadVertex
```cpp
struct QuadVertex {
    glm::vec3 Position;  // 3D position of the vertex
    glm::vec4 Color;     // RGBA color of the vertex
    glm::vec2 TexCoord;  // UV texture coordinates
    float TexIndex;      // Texture slot index for batching
};
```

#### RenderStats
```cpp
struct RenderStats {
    uint32_t DrawCalls = 0;    // Number of draw calls made
    uint32_t QuadCount = 0;    // Number of quads rendered
    uint32_t VertexCount = 0;  // Total number of vertices processed
    uint32_t IndexCount = 0;   // Total number of indices processed
    
    uint32_t GetTotalVertexCount() const;
    uint32_t GetTotalIndexCount() const;
};
```

## BatchRenderer3D

### Overview
High-performance 3D batch renderer with lighting and material support.

### Core Methods

#### Initialization
```cpp
static void Init();
static void Shutdown();
static bool IsInitialized();
```

#### Scene Management
```cpp
static void BeginScene(const PerspectiveCamera& camera);
static void EndScene();
static void Flush();
```

#### Drawing Primitives

##### Cubes
```cpp
static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
static void DrawCube(const glm::vec3& position, const glm::vec3& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
```

##### Spheres
```cpp
static void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color, int segments = 32);
static void DrawSphere(const glm::vec3& position, float radius, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), int segments = 32);
```

##### Cylinders
```cpp
static void DrawCylinder(const glm::vec3& position, float radius, float height, const glm::vec4& color, int segments = 32);
static void DrawCylinder(const glm::vec3& position, float radius, float height, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), int segments = 32);
```

##### Planes
```cpp
static void DrawPlane(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
static void DrawPlane(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
```

### Data Structures

#### Vertex3D
```cpp
struct Vertex3D {
    glm::vec3 Position;  // 3D position of the vertex
    glm::vec4 Color;      // RGBA color of the vertex
    glm::vec2 TexCoord;   // UV texture coordinates
    float TexIndex;       // Texture slot index for batching
    glm::vec3 Normal;     // Surface normal vector
};
```

#### RenderStats3D
```cpp
struct RenderStats3D {
    uint32_t DrawCalls = 0;     // Number of draw calls made
    uint32_t CubeCount = 0;     // Number of cubes rendered
    uint32_t SphereCount = 0;   // Number of spheres rendered
    uint32_t CylinderCount = 0; // Number of cylinders rendered
    uint32_t PlaneCount = 0;    // Number of planes rendered
    uint32_t VertexCount = 0;   // Total number of vertices processed
    uint32_t IndexCount = 0;    // Total number of indices processed
};
```

## Performance Monitoring

### PerformanceProfiler

#### Core Methods
```cpp
// Initialize the profiler
static void Init();

// Shutdown the profiler
static void Shutdown();

// Record a performance scope
static void RecordScope(const std::string& name, double duration);

// Get current performance data
static PerformanceData GetCurrentData();

// Reset all performance data
static void Reset();
```

#### Macros
```cpp
// Profile a scope
ZG_PROFILE_SCOPE("MyOperation");

// Profile the current function
ZG_PROFILE_FUNCTION();
```

### PerformanceBenchmark

#### Core Methods
```cpp
// Initialize the benchmark system
static void Init();

// Shutdown the benchmark system
static void Shutdown();

// Run a single benchmark
static BenchmarkResult RunBenchmark(const TestScenario& scenario);

// Run a suite of benchmarks
static std::vector<BenchmarkResult> RunBenchmarkSuite(const std::vector<TestScenario>& scenarios);

// Run stress test
static std::vector<BenchmarkResult> RunStressTest(const TestScenario& baseScenario, float duration, float loadMultiplier);
```

#### Test Scenarios
```cpp
namespace BenchmarkScenarios {
    // Basic 2D rendering test
    TestScenario Basic2DRendering();
    
    // Basic 3D rendering test
    TestScenario Basic3DRendering();
    
    // High quad count test
    TestScenario HighQuadCount2D(uint32_t quadCount = 10000);
    
    // High object count 3D test
    TestScenario HighObjectCount3D(uint32_t objectCount = 5000);
    
    // Texture switching test
    TestScenario TextureSwitching(uint32_t textureCount = 100);
    
    // Shader switching test
    TestScenario ShaderSwitching(uint32_t shaderCount = 50);
    
    // Memory stress test
    TestScenario MemoryStressTest();
    
    // Particle system stress test
    TestScenario ParticleSystemStress(uint32_t particleCount = 100000);
    
    // Mixed rendering test
    TestScenario MixedRenderingTest();
}
```

### PerformanceMonitorUI

#### Core Methods
```cpp
// Initialize the UI
static void Init();

// Shutdown the UI
static void Shutdown();

// Update performance data
static void Update(float deltaTime);

// Render the UI
static void Render(bool showOverlay = true);

// Update performance data
static void UpdateData(const PerformanceData& data);
```

#### Performance Data Structure
```cpp
struct PerformanceData {
    float currentFPS;      // Current frames per second
    float averageFPS;      // Average FPS over time
    float minFPS;          // Minimum FPS recorded
    float maxFPS;          // Maximum FPS recorded
    float frameTime;       // Current frame time in milliseconds
    size_t memoryUsage;    // Current memory usage in bytes
    double cpuUsage;       // CPU usage percentage
    double gpuUsage;       // GPU usage percentage
    uint32_t drawCalls;   // Number of draw calls
    uint32_t vertices;    // Number of vertices rendered
    uint32_t indices;     // Number of indices rendered
    float renderTime;      // Time spent rendering
    float updateTime;      // Time spent updating
};
```

## Memory Management

### MemoryPool

#### Template Class
```cpp
template<typename T>
class MemoryPool {
public:
    // Constructor
    MemoryPool(size_t initialSize, size_t growthFactor = 2);
    
    // Destructor
    ~MemoryPool();
    
    // Allocate an object
    T* Allocate();
    
    // Deallocate an object
    void Deallocate(T* object);
    
    // Get pool statistics
    PoolStats GetStats() const;
    
    // Clear all objects
    void Clear();
};
```

### RingBuffer

#### Template Class
```cpp
template<typename T>
class RingBuffer {
public:
    // Constructor
    RingBuffer(size_t capacity);
    
    // Destructor
    ~RingBuffer();
    
    // Push data to the buffer
    void Push(const T& item);
    void Push(const T* items, size_t count);
    
    // Pop data from the buffer
    bool Pop(T& item);
    size_t Pop(T* items, size_t count);
    
    // Get buffer information
    size_t Size() const;
    size_t Capacity() const;
    bool IsEmpty() const;
    bool IsFull() const;
    
    // Clear the buffer
    void Clear();
};
```

## Resource Management

### ResourceManager

#### Core Methods
```cpp
// Initialize the resource manager
static void Init();

// Shutdown the resource manager
static void Shutdown();

// Get total memory usage
static size_t GetTotalMemoryUsage();

// Set memory limits
static void SetMemoryLimits(size_t textureMemoryLimit, size_t shaderMemoryLimit);

// Clear all resources
static void ClearAll();
```

### TextureManager

#### Core Methods
```cpp
// Load texture from file
static Ref<Texture2D> LoadTexture(const std::string& filepath);

// Create texture from data
static Ref<Texture2D> CreateTexture(const void* data, uint32_t width, uint32_t height);

// Get texture statistics
static TextureStats GetStats();

// Clear texture cache
static void ClearCache();
```

### ShaderManager

#### Core Methods
```cpp
// Load shader from files
static Ref<Shader> LoadShader(const std::string& name, const std::string& vertexFilepath, const std::string& fragmentFilepath);

// Load shader from source
static Ref<Shader> LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

// Get shader statistics
static ShaderStats GetStats();

// Clear shader cache
static void ClearCache();
```

## Culling System

### CullingSystem

#### Core Methods
```cpp
// Initialize the culling system
static void Init();

// Shutdown the culling system
static void Shutdown();

// Update frustum from camera
static void UpdateFrustum(const PerspectiveCamera& camera);

// Test if object is visible
static bool IsVisible(const glm::vec3& position, float radius);

// Test if bounding box is visible
static bool IsVisible(const glm::vec3& min, const glm::vec3& max);

// Get culling statistics
static CullingStats GetStats();
```

## Usage Examples

### Basic 2D Rendering
```cpp
// Initialize
BatchRenderer2D::Init();

// Create camera
OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);

// Begin scene
BatchRenderer2D::BeginScene(camera);

// Draw objects
BatchRenderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
BatchRenderer2D::DrawCircle({0.0f, 0.0f}, 0.5f, {0.0f, 1.0f, 0.0f, 1.0f});

// End scene
BatchRenderer2D::EndScene();
```

### Performance Monitoring
```cpp
// Profile a function
void MyFunction() {
    ZG_PROFILE_FUNCTION();
    
    // Your code here
}

// Get performance statistics
auto stats = BatchRenderer2D::GetStats();
ZG_CORE_INFO("Draw Calls: {}, Quads: {}", stats.DrawCalls, stats.QuadCount);
```

### Memory Management
```cpp
// Create memory pool
MemoryPool<MyObject> pool(1000);

// Allocate objects
MyObject* obj = pool.Allocate();

// Use object
obj->DoSomething();

// Deallocate when done
pool.Deallocate(obj);
```

---

*Last updated: 2024*
*Zgine Engine Team*
