# 阶段四：资源与实体 (Resources & Entities)

## 概述

本阶段实现了资源加载系统，包括纹理加载和 3D 模型加载功能，为引擎提供了完整的资源管理能力。

## 完成的任务

### 任务 7: 集成 stb_image 和 Assimp

#### 1. stb_image 集成
- **来源**: GitHub (nothings/stb) - 完整库
- **类型**: 单头文件库集合 (header-only)
- **功能**:
  - 图像加载（PNG, JPG, BMP, TGA 等）
  - 支持多种图像格式
  - 自动内存管理
  - 提供 stb_image_write, stb_truetype, stb_image_resize2 等额外功能
- **集成方式**:
  - 通过 CMake FetchContent 自动获取
  - 通过 `#define STB_IMAGE_IMPLEMENTATION` 实现
  - CMake 配置为 interface 库
  - 所有 21+ stb 库均可用

#### 2. Assimp 集成
- **版本**: v5.4.3
- **来源**: GitHub (assimp/assimp)
- **功能**:
  - 3D 模型加载（OBJ, FBX, GLTF, BLEND 等）
  - 支持多种模型格式
  - 自动处理网格、材质、纹理
- **集成方式**:
  - FetchContent 自动下载
  - 配置为静态库
  - 仅启用必要的导入器（OBJ, FBX, GLTF, BLEND）
  - 禁用测试和工具以加快构建

### 任务 8: 实现 Mesh 加载器

#### 1. Texture 类
- **文件**: `src/Renderer/Texture.h/cpp`
- **功能**: OpenGL 纹理封装
- **特性**:
  - 使用 stb_image 加载图像
  - 自动生成 OpenGL 纹理对象
  - 支持纹理绑定到指定槽位
  - 自动资源管理（RAII）
  - 支持垂直翻转（stbi_set_flip_vertically_on_load）
  - 纹理参数配置（过滤、包装模式）

#### 2. Mesh 类
- **文件**: `src/Resources/Mesh.h/cpp`
- **功能**: 3D 网格数据封装
- **数据结构**:
  - `Vertex`: 顶点数据（位置、法线、纹理坐标）
  - `MeshData`: 网格数据集合（顶点、索引、纹理）
- **特性**:
  - 自动设置顶点属性布局
  - 支持多纹理绑定
  - 提供 Draw() 方法进行渲染
  - 使用 shared_ptr 管理资源

#### 3. MeshLoader 类
- **文件**: `src/Resources/MeshLoader.h/cpp`
- **功能**: 使用 Assimp 加载 3D 模型
- **特性**:
  - 支持加载完整模型（多个网格）
  - 支持加载单个网格
  - 递归处理节点层次结构
  - 自动处理顶点、索引、法线、纹理坐标
  - 自动加载材质纹理（漫反射、镜面反射）
  - 错误处理和日志记录

#### 4. VertexArray 增强
- **文件**: `src/Renderer/VertexArray.cpp`
- **更新**:
  - 支持两种顶点布局：
    - 布局 0: 位置 + 颜色（6 floats）
    - 布局 1: 位置 + 法线 + 纹理坐标（8 floats）
  - 根据顶点缓冲区索引自动选择布局

## 技术细节

### 纹理加载流程

1. 使用 stb_image 加载图像文件
2. 设置垂直翻转（OpenGL 坐标系统）
3. 生成 OpenGL 纹理对象
4. 配置纹理参数（过滤、包装）
5. 上传图像数据到 GPU
6. 释放 CPU 端图像数据

### 模型加载流程

1. 使用 Assimp Importer 加载模型文件
2. 应用后处理（三角化、翻转 UV、计算切线空间）
3. 递归遍历场景节点
4. 处理每个网格：
   - 提取顶点数据（位置、法线、纹理坐标）
   - 提取索引数据
   - 加载材质纹理
5. 创建 Mesh 对象并返回

### 顶点数据布局

#### 简单布局（位置 + 颜色）
```
[Position(3) | Color(3)] = 6 floats per vertex
```

#### 完整布局（位置 + 法线 + 纹理坐标）
```
[Position(3) | Normal(3) | TexCoords(2)] = 8 floats per vertex
```

### Assimp 后处理标志

- `aiProcess_Triangulate`: 将所有图元转换为三角形
- `aiProcess_FlipUVs`: 翻转 UV 坐标（适配 OpenGL）
- `aiProcess_CalcTangentSpace`: 计算切线和副切线（用于法线贴图）

## 第三方库集成

### stb (complete library)
- **版本**: master 分支
- **来源**: GitHub (nothings/stb)
- **用途**: 图像加载、字体渲染、图像处理等
- **集成方式**: CMake FetchContent 自动获取
- **可用库**: stb_image, stb_image_write, stb_truetype, stb_image_resize2 等 21+ 库

### Assimp
- **版本**: v5.4.3
- **来源**: GitHub (assimp/assimp)
- **用途**: 3D 模型加载
- **集成方式**: FetchContent
- **配置**:
  - 静态库构建
  - 仅启用必要导入器
  - 禁用测试和工具

## 代码结构

```
src/Renderer/
├── Texture.h/cpp          # 纹理封装

src/Resources/
├── Mesh.h/cpp             # 网格数据封装
└── MeshLoader.h/cpp       # 模型加载器

build/_deps/stb-src/       # stb 库（FetchContent 自动下载）
├── stb_image.h            # 图像加载库
├── stb_image_write.h      # 图像写入库
├── stb_truetype.h         # 字体渲染库
└── ... (21+ 其他 stb 库)
```

## 使用示例

### 加载纹理
```cpp
auto texture = std::make_shared<Texture>("assets/textures/diffuse.png");
texture->Bind(0);  // 绑定到纹理槽位 0
```

### 加载模型
```cpp
// 加载完整模型（可能包含多个网格）
auto meshes = MeshLoader::LoadModel("assets/models/cube.obj");

// 或加载单个网格
auto mesh = MeshLoader::LoadMesh("assets/models/cube.obj");
```

### 渲染网格
```cpp
shader.Bind();
mesh->Draw();  // 自动绑定纹理并绘制
```

## CMake 配置更新

### 新增库
- `stb_image`: Interface 库
- `assimp`: 静态库（通过 FetchContent）

### 新增源文件
- `src/Renderer/Texture.cpp`
- `src/Resources/Mesh.cpp`
- `src/Resources/MeshLoader.cpp`

### 链接配置
```cmake
target_link_libraries(Zgine
    PRIVATE
        stb_image
        assimp
)
```

## 测试验收

### 验收标准
✅ 集成 stb_image 和 Assimp 库
✅ 实现 Texture 类
✅ 实现 Mesh 加载器
✅ 能够加载带纹理的立方体或简单的 .obj 模型

### 测试要点
- 纹理加载功能正常
- 模型文件能够正确解析
- 顶点数据正确提取
- 纹理坐标正确映射
- 多网格模型能够正确处理

## 已知限制

1. **纹理格式**: 目前主要支持常见格式（PNG, JPG 等）
2. **模型格式**: 优先支持 OBJ，其他格式需要相应导入器
3. **材质系统**: 基础实现，完整 PBR 材质待后续实现
4. **性能**: 未实现资源缓存，每次加载都会重新读取文件

## 下一步计划

阶段五将实现：
- 物理引擎集成（Jolt Physics）
- RigidBody 和 Collider 组件
- 物理模拟系统
- 重力、碰撞检测

## 提交记录

- 阶段四任务7: 集成 stb_image 和 Assimp 库
- 阶段四任务8: 实现 Texture 类和 Mesh 加载器
- 阶段四验收: 支持加载带纹理的 3D 模型

## 注意事项

1. **资源路径**: 确保模型和纹理文件路径正确
2. **纹理翻转**: stb_image 会自动翻转纹理以适配 OpenGL 坐标系统
3. **模型格式**: 建议使用 OBJ 格式进行测试，兼容性最好
4. **内存管理**: 使用 shared_ptr 确保资源正确释放
5. **错误处理**: 加载失败时会输出错误日志，注意检查控制台输出

