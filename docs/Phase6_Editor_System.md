# 阶段六：编辑器系统 (Editor System)

## 概述

本阶段实现了基于 ImGui 的可视化编辑器系统，为引擎提供了场景编辑、实体管理和属性检视功能，实现了实时属性编辑和场景可视化编辑能力。

## 完成的任务

### 任务 11: 集成 ImGui

#### 1. ImGui 集成
- **版本**: v1.90.5
- **来源**: GitHub (ocornut/imgui)
- **功能**: 
  - 即时模式 GUI 库
  - 支持多视口和停靠
  - 丰富的控件和布局选项
  - 高性能渲染
- **集成方式**: 
  - 使用 FetchContent 自动下载
  - 手动创建静态库（包含后端实现）
  - 配置 GLFW 和 OpenGL 后端

#### 2. ImGui 后端配置
- **GLFW 后端**: `imgui_impl_glfw.cpp`
- **OpenGL 后端**: `imgui_impl_opengl3.cpp`
- **多视口支持**: 启用 `ImGuiConfigFlags_ViewportsEnable`
- **停靠支持**: 启用 `ImGuiConfigFlags_DockingEnable`

### 任务 12: 实现 Editor 类

#### 1. Editor 类
- **文件**: `src/Editor/Editor.h/cpp`
- **功能**: 编辑器系统核心管理类
- **主要方法**:
  - `Initialize()`: 初始化 ImGui 和编辑器
  - `Shutdown()`: 关闭编辑器
  - `BeginFrame()`: 开始 ImGui 帧
  - `EndFrame()`: 结束 ImGui 帧并渲染
  - `Render()`: 渲染编辑器 UI
  - `GetSelectedEntity()`: 获取选中的实体
  - `SetSelectedEntity()`: 设置选中的实体

#### 2. 编辑器面板

**Hierarchy 面板**:
- 显示场景中所有实体的列表
- 支持实体选择
- 显示实体的 Tag 名称
- 可折叠/展开

**Inspector 面板**:
- 显示选中实体的所有组件
- 支持实时编辑组件属性
- 支持添加/移除组件
- 组件属性可折叠显示

**菜单栏**:
- 窗口管理菜单
- 控制面板显示/隐藏

### 任务 13: 实现组件属性编辑

#### 1. TransformComponent 编辑
- **位置 (Translation)**: DragFloat3 控件
- **旋转 (Rotation)**: DragFloat3 控件（欧拉角）
- **缩放 (Scale)**: DragFloat3 控件
- **实时更新**: 修改立即生效

#### 2. RigidBodyComponent 编辑
- **Body Type**: Combo 选择框（Static/Dynamic/Kinematic）
- **Mass**: DragFloat 控件
- **Friction**: DragFloat 控件（0.0-1.0）
- **Restitution**: DragFloat 控件（0.0-1.0）

#### 3. BoxColliderComponent 编辑
- **Size**: DragFloat3 控件
- **Offset**: DragFloat3 控件

#### 4. TagComponent 编辑
- **Tag**: InputText 控件
- 支持实时修改实体名称

## 技术细节

### ImGui 初始化流程

1. **创建上下文**: `ImGui::CreateContext()`
2. **配置标志**: 启用键盘导航、停靠、多视口
3. **设置样式**: 使用深色主题
4. **初始化后端**: GLFW 和 OpenGL3 后端
5. **设置字体**: 使用默认字体

### 编辑器渲染流程

```
BeginFrame() → Render() → EndFrame()
    ↓            ↓           ↓
NewFrame    Hierarchy    Render
            Inspector    UpdatePlatformWindows
            MenuBar      RenderPlatformWindowsDefault
```

### 实体选择机制

- 使用 `Entity` 的比较操作符判断选中状态
- Hierarchy 面板中使用 `ImGui::Selectable` 实现选择
- 选中状态存储在 `m_SelectedEntity` 成员变量中
- Inspector 面板根据选中实体动态显示组件

### 组件属性同步

- 使用 ImGui 的控件直接修改组件数据
- 修改立即反映到 ECS 系统中
- 物理系统会在下一帧同步更新（如果实体有物理组件）

## 第三方库集成

### ImGui
- **版本**: v1.90.5
- **来源**: GitHub (ocornut/imgui)
- **用途**: GUI 渲染和交互
- **集成方式**: FetchContent + 手动创建库
- **配置**:
  - 静态库构建
  - GLFW 后端
  - OpenGL3 后端
  - 多视口支持
  - 停靠支持

## 代码结构

```
src/Editor/
├── Editor.h          # 编辑器头文件
└── Editor.cpp         # 编辑器实现
    ├── Initialize()      # 初始化 ImGui
    ├── Render()          # 渲染编辑器 UI
    ├── RenderHierarchy() # 渲染 Hierarchy 面板
    ├── RenderInspector() # 渲染 Inspector 面板
    └── Draw*Component()  # 渲染各组件属性
```

## 使用示例

### 初始化编辑器

```cpp
// 创建窗口
Window window(props);

// 创建编辑器
Editor editor;
editor.Initialize(&window);
```

### 主循环集成

```cpp
while (window.IsRunning()) {
    // 编辑器帧开始
    editor.BeginFrame();
    
    // 游戏逻辑更新
    // ...
    
    // 渲染游戏场景
    // ...
    
    // 渲染编辑器
    editor.Render(&scene);
    editor.EndFrame();
    
    window.OnUpdate();
}

// 清理
editor.Shutdown();
```

### 实体选择

```cpp
// 获取选中的实体
Entity selected = editor.GetSelectedEntity();

// 设置选中的实体
editor.SetSelectedEntity(entity);
```

## CMake 配置更新

### 新增库
- `imgui`: GUI 库（通过 FetchContent + 手动构建）

### CMake 配置
```cmake
# ImGui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.5
    GIT_SHALLOW TRUE
)
set(IMGUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(imgui)

# 手动创建 ImGui 库
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
add_library(imgui::imgui ALIAS imgui)

# 链接配置
target_link_libraries(Zgine PRIVATE
    imgui::imgui
)
```

### 新增源文件
- `src/Editor/Editor.cpp`
- `src/Editor/Editor.h`

## 测试验收

### 验收标准
✅ 集成 ImGui 库
✅ 实现 Editor 类
✅ 实现 Hierarchy 面板
✅ 实现 Inspector 面板
✅ 支持实时属性编辑
✅ UI 修改属性实时生效

### 测试场景
1. **Hierarchy 面板测试**:
   - 创建多个实体
   - 在 Hierarchy 面板中查看实体列表
   - 点击实体进行选择
   - 验证选中状态正确显示

2. **Inspector 面板测试**:
   - 选择实体
   - 在 Inspector 面板中查看组件
   - 修改 Transform 属性
   - 验证场景中实体位置实时更新

3. **组件编辑测试**:
   - 修改 RigidBody 的质量
   - 修改 BoxCollider 的尺寸
   - 修改 Tag 名称
   - 验证所有修改立即生效

4. **添加组件测试**:
   - 选择实体
   - 点击 "Add Component" 按钮
   - 添加新组件
   - 验证组件正确添加

## 已知限制

1. **组件类型**: 目前仅支持编辑部分组件（Transform、RigidBody、BoxCollider、Tag）
2. **物理同步**: 修改物理属性后需要重新创建物理体才能生效
3. **实体删除**: 尚未实现实体删除功能
4. **场景保存**: 尚未实现场景保存/加载功能
5. **撤销/重做**: 尚未实现操作历史记录
6. **多选**: 仅支持单选，不支持多选
7. **实体层次**: 尚未实现父子关系显示

## 下一步计划

阶段七将实现：
- 场景序列化系统
- JSON 格式的场景保存/加载
- 场景文件管理
- 资源引用序列化

## 提交记录

- 阶段六任务11: 集成 ImGui 库
- 阶段六任务12: 实现 Editor 类和编辑器系统
- 阶段六任务13: 实现组件属性编辑功能
- 阶段六验收: 支持实时属性编辑，UI 修改属性实时生效

## 注意事项

1. **输入捕获**: 当 ImGui 窗口获得焦点时，游戏输入会被禁用（通过 `io.WantCaptureKeyboard` 检查）
2. **多视口**: 支持多视口模式，可以在不同窗口中显示编辑器
3. **停靠**: 支持面板停靠，可以自由调整布局
4. **性能**: ImGui 渲染对性能影响较小，但大量控件可能影响帧率
5. **内存管理**: ImGui 使用自己的内存管理，无需手动释放
6. **线程安全**: ImGui 不是线程安全的，必须在主线程中调用

## 性能考虑

1. **渲染优化**: ImGui 使用即时模式，每帧重新构建 UI，但性能开销很小
2. **实体遍历**: Hierarchy 面板遍历所有实体，大量实体可能影响性能
3. **属性更新**: 属性修改立即生效，无需额外同步机制
4. **多视口**: 多视口模式会增加一些渲染开销

## 调试技巧

1. **ImGui Demo**: 可以通过 `ImGui::ShowDemoWindow()` 查看所有可用控件
2. **样式编辑**: 可以通过 `ImGui::ShowStyleEditor()` 自定义编辑器样式
3. **性能监控**: ImGui 提供性能指标，可以通过 `io` 对象访问
4. **日志输出**: 编辑器操作会输出日志，便于调试

## UI 布局说明

### 默认布局
- **左上**: Hierarchy 面板（实体列表）
- **右上**: Inspector 面板（属性编辑）
- **底部**: 游戏视图（主渲染窗口）
- **顶部**: 菜单栏

### 自定义布局
- 所有面板都可以拖拽和停靠
- 支持保存/加载布局配置（待实现）
- 支持多窗口显示

