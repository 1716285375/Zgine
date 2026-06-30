# Renderer Rules

版本日期：2026-05-25

## 后端策略

```text
OpenGL    reference backend
Vulkan    cross-platform teaching priority
DirectX12 selectable explicit stub until implemented
None      headless/testing backend
```

## RHI 边界

Public RHI 只能暴露 Zgine 类型：

- `RendererAPI`
- `VertexBuffer`
- `IndexBuffer`
- `VertexArray`
- `BufferLayout`
- `Shader`
- `Texture`
- `Framebuffer`

禁止 public RHI 直接暴露：

- `Vk*`
- `ID3D12*`
- backend 私有 include。

## Vulkan 推进顺序

1. Instance / device / queue / surface / swapchain。
2. Clear-frame command buffer。
3. Resize recreation。
4. Vertex/index buffer resources。
5. Shader modules。
6. Pipeline layout and graphics pipeline。
7. Descriptor sets。
8. Draw command recording。
9. Depth resources。
10. ImGui Vulkan backend。

## 未完成路径

未完成 backend 功能必须：

- 返回 `nullptr` 并记录明确日志，或
- 抛出明确异常，或
- 通过测试声明当前阶段不支持。

不能：

- 静默 no-op 后返回成功。
- 创建半初始化对象。
- 在非 OpenGL backend 偷用 OpenGL 实现。

## Mesh 和 Vertex Layout

通用资源层只描述顶点布局，不调用 backend API。

推荐数据流：

```text
PrimitiveMesh
  -> VertexBuffer::SetLayout(BufferLayout)
  -> VertexArray::AddVertexBuffer
  -> Backend translates layout
```

OpenGL 在 `OpenGLVertexArray` 中转成 `glVertexAttribPointer`。
Vulkan 在后续 pipeline 阶段转成 `VkVertexInputBindingDescription` 和 `VkVertexInputAttributeDescription`。
