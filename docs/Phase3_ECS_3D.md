# 阶段三：ECS 与 3D 空间 (ECS & 3D)

## 概述

本阶段实现了实体组件系统（ECS）架构和 3D 摄像机系统，将之前的三角形渲染转换为基于 ECS 的实体，并实现了第一人称摄像机控制。

## 完成的任务

### 任务 5: 集成 EnTT，创建 Scene 类

#### 1. EnTT 库集成
- **版本**: v3.13.1
- **来源**: GitHub (skypjack/entt)
- **类型**: Header-only 库
- **集成方式**: FetchContent
- **用途**: 提供高性能的 ECS 框架

#### 2. Scene 类
- **文件**: `src/Scene/Scene.h/cpp`
- **功能**: 
  - 管理 EnTT registry
  - 创建和销毁 Entity
  - 提供更新和渲染接口
- **特性**:
  - 使用 `entt::registry` 管理所有实体
  - 提供 Entity 创建和销毁接口

#### 3. Entity 类
- **文件**: `src/Scene/Entity.h`
- **功能**: Entity 的包装类
- **特性**:
  - 提供类型安全的组件操作
  - 支持 AddComponent, GetComponent, HasComponent, RemoveComponent
  - 封装 `entt::entity` 句柄

#### 4. Components (组件)
- **文件**: `src/Scene/Components.h`
- **组件类型**:
  - **TagComponent**: 实体标签/名称
  - **TransformComponent**: 位置、旋转、缩放
    - 提供 `GetTransform()` 方法计算变换矩阵
  - **MeshComponent**: 渲染网格数据
    - 存储 VertexArray 和 IndexBuffer
  - **CameraComponent**: 摄像机投影矩阵和主摄像机标记

### 任务 6: 实现 Camera 系统

#### 1. Camera 类
- **文件**: `src/Core/Camera.h/cpp`
- **功能**: 第一人称摄像机
- **特性**:
  - 支持位置和旋转设置
  - 提供移动方法：MoveForward, MoveRight, MoveUp
  - 提供旋转方法：Rotate (yaw, pitch)
  - 自动计算视图矩阵
  - 使用欧拉角（pitch, yaw）控制方向

#### 2. Input 系统
- **文件**: `src/Core/Input.h/cpp`
- **功能**: 输入处理封装
- **特性**:
  - 键盘按键检测（IsKeyPressed）
  - 鼠标按键检测（IsMouseButtonPressed）
  - 鼠标位置获取（GetMousePosition, GetMouseX, GetMouseY）
  - 基于 GLFW 的输入处理

#### 3. Time 系统
- **文件**: `src/Core/Time.h/cpp`
- **功能**: 时间管理
- **特性**:
  - DeltaTime 计算
  - 总时间记录
  - 静态接口

### 摄像机控制

#### WASD 移动
- **W**: 向前移动
- **S**: 向后移动
- **A**: 向左移动
- **D**: 向右移动
- 移动速度基于 DeltaTime，确保帧率无关

## 第三方库集成

### EnTT (Entity Component System)
- **版本**: v3.13.1
- **来源**: GitHub (skypjack/entt)
- **用途**: ECS 架构核心
- **集成方式**: FetchContent (header-only)

## 测试验收

### 验收标准
✅ 将三角形转化为 Entity
✅ 能够通过键盘 WASD 移动摄像机观察三角形

### 测试结果
- 三角形已转换为 Entity，包含：
  - TagComponent: "Triangle"
  - TransformComponent: 位置 (0, 0, -2)
  - MeshComponent: 包含渲染数据
- 摄像机系统：
  - 初始位置: (0, 0, 0)
  - 投影: 透视投影 (45° FOV)
  - WASD 移动正常工作
  - 视图矩阵正确计算

## 代码结构

```
src/Scene/
├── Scene.h/cpp          # 场景管理类
├── Entity.h             # 实体包装类
└── Components.h         # 组件定义

src/Core/
├── Camera.h/cpp         # 摄像机系统
├── Input.h/cpp          # 输入处理
└── Time.h/cpp           # 时间管理
```

## 技术细节

### ECS 架构
- 使用 EnTT 作为 ECS 核心
- Entity 是 `entt::entity` 的包装
- Components 是纯数据结构（POD）
- Scene 管理所有实体的生命周期

### 摄像机系统
- 使用欧拉角表示旋转（pitch, yaw）
- 基于前方向量、右方向量、上方向量计算视图矩阵
- 使用 `glm::lookAt` 计算视图矩阵
- 俯仰角限制在 -89° 到 89° 之间

### 变换矩阵
- TransformComponent 提供 `GetTransform()` 方法
- 计算顺序：Translation × Rotation × Scale
- 使用 GLM 矩阵运算

### Shader 更新
- 顶点着色器添加了 MVP 矩阵支持
- Uniform: `u_ViewProjection` - 视图投影矩阵
- Uniform: `u_Transform` - 模型变换矩阵

## 渲染流程

1. 清除颜色和深度缓冲
2. 启用深度测试
3. 计算摄像机视图投影矩阵
4. 遍历所有有 MeshComponent 的实体
5. 设置 Shader Uniforms (ViewProjection, Transform)
6. 绑定 VertexArray
7. 绘制元素

## 下一步计划

阶段四将实现：
- 资源加载系统（stb_image, Assimp）
- Mesh 加载器
- 纹理支持
- 加载 3D 模型

## 提交记录

- 阶段三任务5: 集成 EnTT，创建 Scene 类和 Entity 类
- 阶段三任务6: 实现 Camera 系统和 Input 系统
- 阶段三验收: 将三角形转化为 Entity，WASD 移动摄像机

