# SmartPointers 使用示例

本文档展示了 Zgine 智能指针系统的各种使用方法和最佳实践。

## 基础用法

### 创建智能指针

```cpp
#include "Zgine/Core/SmartPointers.h"

// 创建引用计数智能指针
auto texture = Zgine::CreateRef<Texture>("texture.png");
auto shader = Zgine::CreateRef<Shader>("vertex.glsl", "fragment.glsl");

// 创建唯一所有权智能指针
auto camera = Zgine::CreateScope<Camera>();
auto renderer = Zgine::CreateScope<Renderer>();

// 创建弱引用
auto weakTexture = Zgine::CreateWeakRef(texture);
```

### 容器类型

```cpp
// 创建智能指针容器
auto textures = Zgine::CreateRefVector<Texture>(10, "default.png");
auto cameras = Zgine::CreateScopeVector<Camera>(5);

// 使用容器类型别名
Zgine::RefVector<Shader> shaders;
Zgine::ScopeVector<Mesh> meshes;
Zgine::RefMap<std::string, Texture> textureCache;
```

## 高级功能

### 自定义删除器

```cpp
// 使用自定义删除器
auto customTexture = Zgine::CreateRefWithDeleter<Texture>(
    [](Texture* tex) {
        // 自定义清理逻辑
        tex->ReleaseGPUResources();
        delete tex;
    },
    "texture.png"
);

// 使用自定义删除器的唯一指针
auto customRenderer = Zgine::CreateScopeWithDeleter<Renderer>(
    [](Renderer* renderer) {
        renderer->Cleanup();
        delete renderer;
    }
);
```

### 多态支持和类型转换

```cpp
// 基类和派生类
class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void Update() = 0;
};

class Player : public GameObject {
public:
    void Update() override { /* 玩家更新逻辑 */ }
};

class Enemy : public GameObject {
public:
    void Update() override { /* 敌人更新逻辑 */ }
};

// 创建多态对象
auto player = Zgine::CreateRef<Player>();
auto enemy = Zgine::CreateRef<Enemy>();

// 安全类型转换
auto gameObject = Zgine::StaticRefCast<GameObject>(player);
auto backToPlayer = Zgine::DynamicRefCast<Player>(gameObject);

if (backToPlayer) {
    // 转换成功
    backToPlayer->Update();
}
```

### 性能优化功能

```cpp
// 对齐内存分配
auto alignedBuffer = Zgine::CreateAlignedRef<Buffer>(64, 1024); // 64字节对齐

// 使用自定义分配器
std::pmr::monotonic_buffer_resource resource(1024 * 1024); // 1MB
std::pmr::polymorphic_allocator<int> allocator(&resource);
auto customAllocated = Zgine::CreateRefWithAllocator<LargeObject>(allocator);
```

### 实用工具函数

```cpp
// 检查指针有效性
if (Zgine::IsValid(texture)) {
    texture->Bind();
}

// 获取原始指针
Texture* rawPtr = Zgine::GetRawPtr(texture);
if (rawPtr) {
    rawPtr->SetFilter(TextureFilter::Linear);
}

// 重置指针
Zgine::Reset(texture); // texture 现在为 nullptr

// 交换指针
Zgine::Swap(texture1, texture2);
```

### 调试和诊断

```cpp
// 获取引用计数信息
auto refInfo = Zgine::GetRefCountInfo(texture);
std::cout << "Use count: " << refInfo.use_count << std::endl;
std::cout << "Weak count: " << refInfo.weak_count << std::endl;

// 检查循环引用
if (Zgine::HasCircularReference(texture)) {
    std::cout << "Warning: Potential circular reference detected!" << std::endl;
}
```

### 异常安全和RAII

```cpp
// 使用智能指针守卫
void ProcessTexture(Zgine::Ref<Texture>& texture) {
    auto guard = Zgine::CreateGuard(texture); // 自动恢复原始值
    
    // 临时修改
    texture = Zgine::CreateRef<Texture>("temp.png");
    
    // 处理逻辑...
    
    // guard 析构时自动恢复原始 texture 值
}
```

## 最佳实践

### 1. 选择合适的智能指针类型

```cpp
// 使用 Ref<T> 当需要共享所有权时
class ResourceManager {
private:
    Zgine::RefMap<std::string, Texture> m_Textures;
public:
    Zgine::Ref<Texture> LoadTexture(const std::string& path) {
        auto it = m_Textures.find(path);
        if (it != m_Textures.end()) {
            return it->second; // 返回共享引用
        }
        
        auto texture = Zgine::CreateRef<Texture>(path);
        m_Textures[path] = texture;
        return texture;
    }
};

// 使用 Scope<T> 当需要唯一所有权时
class Renderer {
private:
    Zgine::Scope<Shader> m_Shader;
public:
    void SetShader(Zgine::Scope<Shader> shader) {
        m_Shader = std::move(shader); // 转移所有权
    }
};
```

### 2. 避免循环引用

```cpp
// 错误：循环引用
class Node {
public:
    Zgine::Ref<Node> m_Parent;
    Zgine::RefVector<Node> m_Children;
    
    void SetParent(Zgine::Ref<Node> parent) {
        m_Parent = parent; // 可能导致循环引用
        parent->m_Children.push_back(Zgine::CreateRef<Node>(*this));
    }
};

// 正确：使用弱引用打破循环
class Node {
public:
    Zgine::WeakRef<Node> m_Parent; // 使用弱引用
    Zgine::RefVector<Node> m_Children;
    
    void SetParent(Zgine::Ref<Node> parent) {
        m_Parent = Zgine::CreateWeakRef(parent);
        parent->m_Children.push_back(Zgine::CreateRef<Node>(*this));
    }
    
    Zgine::Ref<Node> GetParent() const {
        return m_Parent.lock(); // 安全获取强引用
    }
};
```

### 3. 性能考虑

```cpp
// 预分配容器大小
Zgine::RefVector<GameObject> objects;
objects.reserve(1000); // 避免多次重新分配

// 使用移动语义
Zgine::Scope<LargeObject> CreateLargeObject() {
    auto obj = Zgine::CreateScope<LargeObject>();
    // ... 初始化 ...
    return obj; // 移动返回，避免拷贝
}

// 批量创建
auto textures = Zgine::CreateRefVector<Texture>(100, "default.png");
```

### 4. 错误处理

```cpp
// 安全的指针访问
void ProcessTexture(Zgine::Ref<Texture> texture) {
    if (!Zgine::IsValid(texture)) {
        throw std::invalid_argument("Invalid texture pointer");
    }
    
    // 安全访问
    texture->Bind();
}

// 异常安全的资源管理
class ResourceHolder {
private:
    Zgine::Ref<Texture> m_Texture;
    
public:
    void LoadTexture(const std::string& path) {
        try {
            m_Texture = Zgine::CreateRef<Texture>(path);
        } catch (const std::exception& e) {
            // 处理加载失败
            m_Texture = nullptr;
            throw;
        }
    }
};
```

## 总结

Zgine 的智能指针系统提供了：

1. **类型安全**：编译时类型检查
2. **内存安全**：自动内存管理
3. **性能优化**：对齐分配、自定义分配器
4. **调试支持**：引用计数信息、循环引用检测
5. **异常安全**：RAII 保证
6. **易用性**：简洁的 API 和丰富的工具函数

通过合理使用这些功能，可以构建安全、高效、易维护的 C++ 应用程序。
