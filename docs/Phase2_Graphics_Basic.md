# 阶段二：图形基础 (Graphics Basic)

## 概述

本阶段实现了基础的图形渲染功能，包括 OpenGL 缓冲区和着色器的封装，以及一个简单的彩色三角形渲染示例。

## 完成的任务

### 任务 3: 封装渲染基础类

#### 1. VertexBuffer (顶点缓冲区)
- **文件**: `src/Renderer/VertexBuffer.h/cpp`
- **功能**: 封装 OpenGL 顶点缓冲区对象 (VBO)
- **特性**:
  - 支持静态数据上传
  - 自动资源管理（RAII）
  - Bind/Unbind 接口

#### 2. IndexBuffer (索引缓冲区)
- **文件**: `src/Renderer/IndexBuffer.h/cpp`
- **功能**: 封装 OpenGL 索引缓冲区对象 (EBO)
- **特性**:
  - 支持索引数据上传
  - 记录索引数量
  - Bind/Unbind 接口

#### 3. VertexArray (顶点数组对象)
- **文件**: `src/Renderer/VertexArray.h/cpp`
- **功能**: 封装 OpenGL 顶点数组对象 (VAO)
- **特性**:
  - 管理顶点属性布局
  - 当前支持：位置 (vec3) + 颜色 (vec3)
  - 自动设置顶点属性指针

#### 4. Shader (着色器)
- **文件**: `src/Renderer/Shader.h/cpp`
- **功能**: 封装 OpenGL 着色器程序
- **特性**:
  - 支持顶点和片元着色器编译
  - 着色器错误检查
  - Uniform 设置接口（1i, 1f, 3f, 4f, mat4）
  - 使用 GLM 矩阵类型

### 任务 4: 编写基础 GLSL Shader

#### 顶点着色器 (Basic.vert)
- **文件**: `assets/shaders/Basic.vert`
- **功能**: 
  - 接收位置 (a_Position) 和颜色 (a_Color) 属性
  - 输出颜色到片元着色器
  - 使用 OpenGL 3.3 Core Profile

#### 片元着色器 (Basic.frag)
- **文件**: `assets/shaders/Basic.frag`
- **功能**:
  - 接收顶点着色器传递的颜色
  - 输出最终颜色

### 辅助工具

#### FileUtils (文件工具)
- **文件**: `src/Core/FileUtils.h/cpp`
- **功能**: 读取文件内容（用于加载 Shader 源码）

## 第三方库集成

### GLM (OpenGL Mathematics)
- **版本**: 0.9.9.8
- **来源**: GitHub (g-truc/glm)
- **用途**: 数学库，提供向量、矩阵、四元数等
- **集成方式**: FetchContent (header-only)

## 测试验收

### 验收标准
✅ 屏幕中央显示一个彩色三角形

### 测试结果
- 三角形顶点颜色：
  - 左下角：红色 (1.0, 0.0, 0.0)
  - 右下角：绿色 (0.0, 1.0, 0.0)
  - 顶部：蓝色 (0.0, 0.0, 1.0)
- 三角形位置：屏幕中央
- 背景颜色：深灰色 (0.1, 0.1, 0.1)

## 代码结构

```
src/Renderer/
├── VertexBuffer.h/cpp    # 顶点缓冲区封装
├── IndexBuffer.h/cpp     # 索引缓冲区封装
├── VertexArray.h/cpp     # 顶点数组对象封装
└── Shader.h/cpp          # 着色器程序封装

assets/shaders/
├── Basic.vert            # 基础顶点着色器
└── Basic.frag            # 基础片元着色器

src/Core/
└── FileUtils.h/cpp       # 文件读取工具
```

## 技术细节

### 顶点数据布局
当前实现的顶点数据格式：
```cpp
struct Vertex {
    float position[3];  // x, y, z
    float color[3];     // r, g, b
};
```

### OpenGL 状态管理
- 使用 RAII 模式自动管理 OpenGL 资源
- 所有缓冲区对象在析构时自动释放
- 着色器程序在析构时自动删除

### 错误处理
- Shader 编译错误会输出到日志
- Uniform 不存在时会输出警告
- 文件读取失败会返回空字符串

## 下一步计划

阶段三将实现：
- ECS 架构集成 (EnTT)
- 3D 摄像机系统
- 实体组件系统
- 将三角形转换为 Entity

## 提交记录

- 阶段二任务3: 封装 VertexBuffer, IndexBuffer, VertexArray, Shader
- 阶段二任务4: 编写基础 GLSL Shader
- 阶段二验收: 测试显示彩色三角形

