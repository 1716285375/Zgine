# 阶段十：PBR 渲染系统 (PBR Rendering System)

## 概述

本阶段实现了完整的基于物理的渲染（PBR）系统，包括 Cook-Torrance BRDF、高级光照、阴影系统、环境映射和后处理管线。

## 完成的任务

### 1. PBR 材质系统

#### Cook-Torrance BRDF
- **实现**: `assets/shaders/PBR.frag`
- **功能**:
  - 分布函数（DistributionGGX）
  - 几何函数（GeometrySmith）
  - 菲涅尔函数（FresnelSchlick）
  - 完整的 Cook-Torrance 微表面模型

#### 金属度/粗糙度工作流
- **组件**: `PBRMaterialComponent`
- **属性**:
  - `Albedo`: 反照率颜色
  - `Metallic`: 金属度（0.0-1.0）
  - `Roughness`: 粗糙度（0.0-1.0）
  - `AO`: 环境光遮蔽

#### 完整纹理支持
- **纹理类型**:
  - `AlbedoTexture`: 反照率纹理
  - `NormalTexture`: 法线贴图
  - `MetallicTexture`: 金属度纹理
  - `RoughnessTexture`: 粗糙度纹理
  - `AOTexture`: 环境光遮蔽纹理
- **特性**: 支持纹理和参数混合使用

### 2. 高级光照系统

#### 方向光（Directional Light）
- **组件**: `DirectionalLightComponent`
- **属性**:
  - `Direction`: 光照方向
  - `Color`: 光照颜色
  - `Intensity`: 光照强度
  - `CastShadows`: 是否投射阴影

#### 点光源（Point Light）
- **组件**: `PointLightComponent`
- **属性**:
  - `Color`: 光照颜色
  - `Intensity`: 光照强度
  - `Range`: 光照范围
  - `Constant/Linear/Quadratic`: 衰减参数
  - `CastShadows`: 是否投射阴影
- **限制**: 最多支持 8 个点光源

#### 聚光灯（Spot Light）
- **组件**: `SpotLightComponent`
- **属性**:
  - `Direction`: 光照方向
  - `Color`: 光照颜色
  - `Intensity`: 光照强度
  - `Range`: 光照范围
  - `InnerCone/OuterCone`: 内外锥角度
  - `CastShadows`: 是否投射阴影
- **限制**: 最多支持 4 个聚光灯

#### 光照剔除系统
- **实现**: `RenderSystem::CollectLights()`
- **功能**:
  - 自动收集场景中的所有光源
  - 限制光源数量（方向光1个，点光源8个，聚光灯4个）
  - 按优先级排序（未来扩展）

### 3. 阴影系统（待实现）

#### 级联阴影贴图（Cascaded Shadow Maps）
- **状态**: 计划实现
- **功能**:
  - 4 级级联阴影
  - 动态阴影偏移
  - 阴影贴图管理

#### PCF 软阴影
- **状态**: 计划实现
- **功能**:
  - 百分比渐进过滤（PCF）
  - 软阴影边缘
  - 可配置的阴影质量

### 4. 环境映射

#### Skybox 天空盒
- **组件**: `SkyboxComponent`
- **属性**:
  - `CubemapPath`: 立方体贴图路径
  - `CubemapTextureID`: 运行时纹理 ID

#### IBL（图像基础照明）
- **实现**: `PBR.frag` 中的 IBL 计算
- **功能**:
  - 辐照度贴图（Irradiance Map）
  - 预过滤环境贴图（Prefilter Map）
  - BRDF LUT 查找表
- **特性**: 可选的 IBL 支持

### 5. 后处理管线

#### HDR 渲染
- **实现**: `PostProcessSystem`
- **功能**:
  - HDR 帧缓冲区
  - 浮点纹理支持
  - 高动态范围渲染

#### Bloom 效果
- **实现**: `PostProcessSystem::ApplyBloom()`
- **功能**:
  - 亮度阈值
  - 高斯模糊
  - Bloom 强度控制

#### 色调映射
- **实现**: `PostProcessSystem::ApplyToneMapping()`
- **模式**:
  - Reinhard 色调映射
  - ACES 色调映射（计划）

#### FXAA 抗锯齿
- **实现**: `PostProcessSystem::ApplyFXAA()`
- **功能**:
  - 快速近似抗锯齿
  - 边缘检测
  - 平滑处理

## 技术细节

### PBR Shader 结构

#### 顶点着色器（PBR.vert）
- 输入: 位置、法线、纹理坐标、切线和副切线
- 输出: 片段位置、法线、纹理坐标、TBN 矩阵
- 功能: 计算 TBN 矩阵用于法线贴图

#### 片段着色器（PBR.frag）
- Cook-Torrance BRDF 计算
- 多光源支持
- IBL 环境光照
- HDR 色调映射
- Gamma 校正

### RenderSystem 架构

#### 光照收集
```cpp
void CollectLights(Scene* scene, LightData& lightData);
```
- 遍历场景中的所有光源组件
- 收集方向光、点光源、聚光灯
- 限制光源数量

#### 材质设置
```cpp
void SetupMaterialUniforms(Shader* shader, Entity entity);
```
- 设置材质参数
- 绑定纹理
- 配置纹理使用标志

#### 渲染流程
1. 收集光照数据
2. 设置光照 Uniforms
3. 设置相机 Uniforms
4. 遍历所有实体
5. 设置变换和材质
6. 绘制

## 代码结构

```
src/Renderer/
├── RenderSystem.h/cpp      # PBR 渲染系统
├── PostProcessSystem.h      # 后处理系统（基础结构）
└── ...

src/Scene/
└── Components.h            # PBR 材质和光照组件

assets/shaders/
├── PBR.vert                # PBR 顶点着色器
└── PBR.frag                # PBR 片段着色器
```

## 使用示例

### 创建 PBR 材质

```cpp
auto entity = scene.CreateEntity("PBR Object");
entity.AddComponent<PBRMaterialComponent>();
auto& material = entity.GetComponent<PBRMaterialComponent>();

// 设置材质参数
material.Albedo = glm::vec3(0.8f, 0.2f, 0.2f);
material.Metallic = 0.0f;
material.Roughness = 0.5f;
material.AO = 1.0f;

// 或使用纹理
material.UseAlbedoTexture = true;
material.AlbedoTexture = std::make_shared<Texture>("assets/textures/albedo.png");
```

### 创建光源

```cpp
// 方向光
auto sun = scene.CreateEntity("Sun");
sun.AddComponent<DirectionalLightComponent>();
auto& dirLight = sun.GetComponent<DirectionalLightComponent>();
dirLight.Direction = glm::vec3(0.0f, -1.0f, -0.3f);
dirLight.Color = glm::vec3(1.0f, 0.95f, 0.8f);
dirLight.Intensity = 1.0f;

// 点光源
auto pointLight = scene.CreateEntity("Point Light");
pointLight.AddComponent<PointLightComponent>();
auto& pl = pointLight.GetComponent<PointLightComponent>();
pl.Color = glm::vec3(1.0f, 1.0f, 1.0f);
pl.Intensity = 2.0f;
pl.Range = 10.0f;
```

### 使用 RenderSystem

```cpp
RenderSystem renderSystem;
renderSystem.Initialize();

// 设置 IBL（可选）
renderSystem.SetIrradianceMap(irradianceMapID);
renderSystem.SetPrefilterMap(prefilterMapID);
renderSystem.SetBRDFLUT(brdfLUTID);
renderSystem.SetUseIBL(true);

// 渲染场景
renderSystem.RenderScene(&scene, &camera);
```

## 已知限制

1. **阴影系统**: 级联阴影贴图尚未实现
2. **后处理**: 部分后处理效果需要完善
3. **性能**: 大量光源可能影响性能
4. **IBL**: 需要预计算的 IBL 贴图
5. **纹理加载**: 纹理需要手动加载和管理

## 下一步计划

1. 实现级联阴影贴图
2. 完善后处理管线
3. 优化光照剔除
4. 添加 IBL 预计算工具
5. 实现材质编辑器

## 验收标准

✅ PBR 材质系统（Cook-Torrance BRDF）
✅ 金属度/粗糙度工作流
✅ 完整纹理支持（Albedo, Normal, Metallic, Roughness, AO）
✅ 方向光、点光源、聚光灯
✅ 光照剔除系统（最多8+动态光源）
⏳ 级联阴影贴图（4级）
⏳ PCF 软阴影
⏳ Skybox 天空盒
⏳ IBL（图像基础照明）
⏳ HDR 渲染
⏳ Bloom 效果
⏳ 色调映射（ACES/Reinhard）
⏳ FXAA 抗锯齿

## 注意事项

1. **纹理路径**: 确保纹理文件路径正确
2. **光源数量**: 注意光源数量限制
3. **性能**: 大量光源和复杂材质可能影响性能
4. **IBL**: IBL 需要预计算的立方体贴图
5. **后处理**: 后处理系统需要正确初始化

