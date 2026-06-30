# Requirements

## Functional Requirements

1. Runtime target 不包含 Editor 源文件。
2. EditorCore target 可以依赖 Runtime。
3. Sandbox 不依赖 Editor。
4. Editor tests 可以独立验证 EditorCore 行为。
5. 文档明确 Runtime/Editor 允许和禁止依赖方向。

## Non-Functional Requirements

1. 不引入循环依赖。
2. 不让 Runtime public header include Editor header。
3. CMake 新增目录时必须检查 target 归属。
4. 相关测试必须纳入 CTest。
