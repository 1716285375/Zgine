# Requirements

## Functional Requirements

1. 定义或复用 `AssetType`。
2. 定义 AssetDatabase 可查询的 asset record。
3. 支持扫描 `assets/`。
4. 支持 folder、texture、model、scene、material、script、audio 的基础分类。
5. 支持按 path 查询 metadata。
6. 支持按 type 过滤。

## Non-Functional Requirements

1. Runtime 不依赖 Editor。
2. 文件扫描逻辑不能写在 UI 层。
3. 扫描结果应可测试。
4. 后续能扩展缩略图、搜索、导入状态和热重载。
