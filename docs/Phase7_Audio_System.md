# 阶段七：音频系统 (Audio System)

## 概述

本阶段实现了基于 miniaudio 的音频系统，为引擎提供了完整的音频播放、3D 空间音频和音频管理功能，实现了音频与 ECS、Transform、Camera 的同步。

## 完成的任务

### 任务 14: 集成 miniaudio

#### 1. miniaudio 集成
- **版本**: v0.11.23
- **来源**: GitHub (mackron/miniaudio)
- **功能**: 
  - 跨平台音频库
  - 支持多种音频格式（MP3, WAV, OGG, FLAC 等）
  - 3D 空间音频支持
  - 低延迟音频播放
  - 自动设备管理
- **集成方式**: 
  - 使用 FetchContent 自动下载
  - 手动创建静态库（单文件实现）
  - 配置监听器支持

#### 2. miniaudio 配置
- **监听器数量**: 1 个（支持 3D 空间音频）
- **设备管理**: 自动选择默认音频设备
- **格式支持**: 自动解码多种音频格式

### 任务 15: 实现 AudioSystem

#### 1. AudioSystem 类
- **文件**: `src/Audio/AudioSystem.h/cpp`
- **功能**: 音频系统核心管理类
- **主要方法**:
  - `Initialize()`: 初始化音频引擎
  - `Shutdown()`: 关闭音频引擎
  - `OnSceneStart()`: 场景启动时调用
  - `OnSceneStop()`: 场景停止时调用
  - `Update()`: 更新音频系统（同步位置等）
  - `CreateAudioSource()`: 创建音频源
  - `DestroyAudioSource()`: 销毁音频源
  - `PlayAudioSource()`: 播放音频
  - `StopAudioSource()`: 停止音频
  - `PauseAudioSource()`: 暂停音频
  - `UpdateAudioSourcePosition()`: 更新音频源位置
  - `UpdateListener()`: 更新监听器位置和方向

#### 2. 音频源管理

**创建音频源**:
- 从文件路径加载音频
- 配置音量、音调、循环等属性
- 支持 3D 空间音频配置
- 自动设置最小/最大距离

**音频播放控制**:
- 播放、停止、暂停功能
- 实时音量控制
- 音调调整
- 循环播放

**3D 空间音频**:
- 根据音频源位置计算音量衰减
- 根据监听器位置计算左右声道
- 支持最小/最大距离配置
- 实时位置同步

### 任务 16: 实现音频组件

#### 1. AudioSourceComponent
- **文件**: `src/Scene/Components.h`
- **功能**: 音频源组件
- **属性**:
  - `FilePath`: 音频文件路径
  - `IsPlaying`: 是否正在播放
  - `IsLooping`: 是否循环播放
  - `Volume`: 音量（0.0-1.0）
  - `Pitch`: 音调（1.0 为正常）
  - `MinDistance`: 最小距离（3D 音频）
  - `MaxDistance`: 最大距离（3D 音频）
  - `Spatialized`: 是否启用 3D 空间音频
  - `RuntimeSourcePtr`: 运行时指向 miniaudio 源的指针
- **特性**:
  - 纯数据结构，符合 ECS 设计原则
  - 支持 2D 和 3D 音频
  - 可配置音频属性

#### 2. AudioListenerComponent
- **文件**: `src/Scene/Components.h`
- **功能**: 音频监听器组件
- **属性**:
  - 无额外属性（使用 Transform 和 Camera 组件提供位置和方向）
- **特性**:
  - 通常与 Camera 组件一起使用
  - 用于 3D 空间音频的监听器定位
  - 支持多个监听器（当前实现支持 1 个）

## 技术细节

### miniaudio 架构

#### 引擎初始化
- **ma_engine**: 音频引擎核心对象
- **ma_engine_config**: 引擎配置
- **监听器配置**: 支持 1 个监听器用于 3D 音频

#### 音频源管理
- **ma_sound**: 音频源对象
- **文件加载**: 使用 `ma_sound_init_from_file` 从文件加载
- **异步加载**: 支持异步加载标志
- **资源管理**: 自动管理音频资源生命周期

#### 3D 空间音频
- **位置同步**: 音频源位置与 TransformComponent 同步
- **监听器同步**: 监听器位置与 Camera/Transform 同步
- **方向计算**: 从旋转计算前方向量
- **距离衰减**: 根据距离自动计算音量衰减
- **左右声道**: 根据相对位置计算左右声道平衡

### 音频与 ECS 同步

#### Transform 同步
- 音频源位置从 `TransformComponent.Translation` 获取
- 监听器位置从 `TransformComponent.Translation` 获取
- 监听器方向从 `TransformComponent.Rotation` 计算

#### Camera 同步
- 监听器通常与主摄像机绑定
- 如果没有 AudioListenerComponent，使用主摄像机作为监听器
- 摄像机旋转用于计算监听器方向

#### 更新流程
```
Update() → UpdateListener() → UpdateAudioSourcePosition()
    ↓              ↓                      ↓
同步监听器位置    同步音频源位置        更新所有音频源
```

## 第三方库集成

### miniaudio
- **版本**: v0.11.23
- **来源**: GitHub (mackron/miniaudio)
- **用途**: 音频播放和 3D 空间音频
- **集成方式**: FetchContent + 手动创建库
- **配置**:
  - 静态库构建
  - 单文件实现（miniaudio.c）
  - 监听器支持
  - 自动格式解码

## 代码结构

```
src/Audio/
├── AudioSystem.h          # 音频系统头文件
└── AudioSystem.cpp         # 音频系统实现

src/Scene/
└── Components.h            # 音频组件定义
    ├── AudioSourceComponent
    └── AudioListenerComponent
```

## 使用示例

### 创建音频源

```cpp
// 创建 BGM 实体
auto bgmEntity = scene.CreateEntity("BGM");
auto& bgmSource = bgmEntity.AddComponent<AudioSourceComponent>("assets/sounds/bgm.mp3");
bgmSource.IsPlaying = true;
bgmSource.IsLooping = true;
bgmSource.Volume = 0.5f;
bgmSource.Spatialized = false; // BGM 不使用 3D 空间音频

// 创建音频源并播放
audioSystem.CreateAudioSource(bgmEntity);
audioSystem.PlayAudioSource(bgmEntity);
```

### 创建 3D 音频源

```cpp
// 创建 3D 音频源
auto soundEntity = scene.CreateEntity("Sound");
auto& soundSource = soundEntity.AddComponent<AudioSourceComponent>("assets/sounds/effect.wav");
soundSource.IsPlaying = true;
soundSource.Volume = 1.0f;
soundSource.Spatialized = true; // 启用 3D 空间音频
soundSource.MinDistance = 1.0f;
soundSource.MaxDistance = 100.0f;

// 设置位置
auto& transform = soundEntity.GetComponent<TransformComponent>();
transform.Translation = glm::vec3(5.0f, 0.0f, 0.0f);

// 创建并播放
audioSystem.CreateAudioSource(soundEntity);
audioSystem.PlayAudioSource(soundEntity);
```

### 创建音频监听器

```cpp
// 为摄像机添加音频监听器
auto cameraEntity = scene.CreateEntity("Camera");
cameraEntity.AddComponent<CameraComponent>(projection);
cameraEntity.AddComponent<AudioListenerComponent>();

// 设置摄像机位置（监听器位置会自动同步）
auto& transform = cameraEntity.GetComponent<TransformComponent>();
transform.Translation = glm::vec3(0.0f, 2.0f, 8.0f);
```

### 音频系统初始化

```cpp
// 初始化音频系统
AudioSystem audioSystem;
audioSystem.Initialize();
audioSystem.OnSceneStart(&scene);

// 主循环中更新
while (window.IsRunning()) {
    float deltaTime = GetDeltaTime();
    
    // 更新音频系统（同步位置等）
    audioSystem.Update(&scene, deltaTime);
    
    // 其他更新...
}

// 清理
audioSystem.OnSceneStop();
audioSystem.Shutdown();
```

## CMake 配置更新

### 新增库
- `miniaudio`: 音频库（通过 FetchContent）

### CMake 配置
```cmake
# miniaudio
FetchContent_Declare(miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.23
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(miniaudio)

# 手动创建 miniaudio 库
if(TARGET miniaudio)
    target_include_directories(miniaudio PUBLIC ${miniaudio_SOURCE_DIR})
    if(NOT TARGET miniaudio::miniaudio)
        add_library(miniaudio::miniaudio ALIAS miniaudio)
    endif()
elseif(NOT TARGET miniaudio::miniaudio)
    add_library(miniaudio STATIC ${miniaudio_SOURCE_DIR}/miniaudio.c)
    target_include_directories(miniaudio PUBLIC ${miniaudio_SOURCE_DIR})
    add_library(miniaudio::miniaudio ALIAS miniaudio)
endif()

# 链接配置
target_link_libraries(Zgine PRIVATE
    miniaudio::miniaudio
)
```

### 新增源文件
- `src/Audio/AudioSystem.cpp`
- `src/Audio/AudioSystem.h`

## 测试验收

### 验收标准
✅ 集成 miniaudio 库
✅ 实现 AudioSystem 类
✅ 实现 AudioSourceComponent 和 AudioListenerComponent
✅ 支持音频播放、停止、暂停
✅ 支持 3D 空间音频
✅ 音频与 ECS、Transform、Camera 同步
✅ 进入场景自动播放 BGM
✅ 移动摄像机，左右声道变化明显（3D Spatial Audio）

### 测试场景
1. **BGM 播放测试**:
   - 创建 BGM 音频源
   - 设置循环播放
   - 验证进入场景时自动播放
   - 验证循环播放功能

2. **3D 空间音频测试**:
   - 创建 3D 音频源
   - 设置音频源位置
   - 移动摄像机
   - 验证左右声道变化
   - 验证距离衰减效果

3. **音频控制测试**:
   - 测试播放、停止、暂停功能
   - 测试音量调整
   - 测试音调调整
   - 验证所有控制功能正常

4. **位置同步测试**:
   - 移动音频源实体
   - 验证音频位置实时更新
   - 移动摄像机（监听器）
   - 验证监听器位置实时更新

## 已知限制

1. **监听器数量**: 当前实现仅支持 1 个监听器
2. **音频格式**: 依赖 miniaudio 支持的格式，某些格式可能需要额外解码器
3. **性能优化**: 大量音频源可能影响性能，未实现音频源池化
4. **音频混合**: 未实现音频混合和优先级管理
5. **音频效果**: 未实现混响、回声等音频效果
6. **资源管理**: 音频资源未实现缓存和重用机制
7. **异步加载**: 虽然支持异步加载标志，但未实现完整的异步加载流程

## 下一步计划

阶段八将实现：
- 场景序列化系统
- JSON 格式的场景保存/加载
- 场景文件管理
- 资源引用序列化
- 场景状态完整恢复

## 提交记录

- 阶段七任务14: 集成 miniaudio 库
- 阶段七任务15: 实现 AudioSystem 类
- 阶段七任务16: 实现 AudioSourceComponent 和 AudioListenerComponent
- 阶段七验收: 支持音频播放和 3D 空间音频，进入场景自动播放 BGM，移动摄像机左右声道变化明显

## 注意事项

1. **音频文件路径**: 确保音频文件路径正确，支持相对路径和绝对路径
2. **音频格式**: 推荐使用 MP3、WAV 等常见格式，确保格式被 miniaudio 支持
3. **3D 音频**: 3D 空间音频需要音频源和监听器都有正确的位置信息
4. **资源管理**: 音频源销毁时会自动释放资源，无需手动管理
5. **线程安全**: miniaudio 是线程安全的，但当前实现使用单线程模式
6. **坐标系统**: 使用右手坐标系，与 OpenGL 一致
7. **距离衰减**: 距离衰减基于最小/最大距离配置，可根据需求调整
8. **监听器方向**: 监听器方向从 Transform 的旋转计算，确保旋转值正确

## 性能考虑

1. **音频源数量**: 大量音频源可能影响性能，建议限制同时播放的音频数量
2. **文件加载**: 音频文件加载是同步的，大文件可能造成卡顿
3. **位置更新**: 每帧更新所有音频源位置，大量音频源可能影响性能
4. **内存使用**: 音频文件会加载到内存，注意内存使用情况
5. **CPU 使用**: 音频解码和 3D 音频计算会占用 CPU 资源

## 调试技巧

1. **日志输出**: 音频系统初始化、创建/销毁音频源时会输出日志
2. **音量检查**: 如果听不到声音，检查音量设置和音频文件路径
3. **3D 音频调试**: 使用日志输出音频源和监听器位置，验证位置同步
4. **格式支持**: 如果音频无法加载，检查音频格式是否被支持
5. **设备检查**: 确保系统有可用的音频输出设备

## 音频文件格式支持

miniaudio 支持以下音频格式：
- **MP3**: MPEG Audio Layer III
- **WAV**: Waveform Audio File Format
- **OGG**: Ogg Vorbis
- **FLAC**: Free Lossless Audio Codec
- **其他**: 根据 miniaudio 版本支持更多格式

## 3D 空间音频原理

### 距离衰减
- **最小距离**: 在此距离内音量最大
- **最大距离**: 超过此距离音量衰减为 0
- **衰减曲线**: 线性衰减（可根据需求调整）

### 左右声道
- **相对位置**: 根据音频源相对于监听器的位置计算
- **方向向量**: 使用监听器的前方向量计算左右声道
- **平衡计算**: 根据相对位置计算左右声道音量比例

### 方向计算
- **前方向量**: 从旋转（欧拉角）计算前方向量
- **上方向量**: 默认使用 (0, 1, 0) 作为世界坐标系上方向
- **方向同步**: 每帧更新监听器方向

