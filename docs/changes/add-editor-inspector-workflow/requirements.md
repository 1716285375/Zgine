# Requirements

## Functional Requirements

1. Inspector 根据当前 selection 类型显示内容。
2. Entity selection 显示 Tag、Transform、核心渲染/物理/音频/脚本组件。
3. Asset selection 显示路径、类型、handle、import 状态。
4. 修改 Transform 等可撤销字段必须走 command。
5. Inspector 可扩展新的 component drawer。

## Non-Functional Requirements

1. Runtime 不依赖 Editor。
2. Inspector 不直接调用 OpenGL/Vulkan。
3. UI 层不保存长期业务状态。
4. Editor tests 覆盖 selection 和 command 行为。
