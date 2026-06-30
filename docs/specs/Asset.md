# Spec: Asset

版本日期：2026-05-29

## 职责

Asset 模块负责 AssetHandle、AssetMetadata、AssetDatabase、AssetManager、Importer、Cache 和后续 Hot Reload。

## 不负责

- 不直接修改 World。
- 不依赖 Editor panel。
- 不在后台线程直接创建 OpenGL 对象。

## 规则

- CPU import 可以后台执行。
- Cache/metadata 提交必须同步。
- GPU 资源创建必须有明确线程归属。
- 资源路径语义要统一，避免 File/VFS/absolute path 三套逻辑互相冲突。
- AssetDatabase 只维护资源清单、类型、路径和 metadata handle，不直接加载 GPU/Audio/Physics 运行时对象。
- AssetDatabase 扫描结果必须稳定排序，便于测试、编辑器显示和 AI 对比 diff。
- AssetDatabase 可以接受绝对路径或相对 assets root 的查询，但不能把 Editor 选择状态写进 Runtime 资源层。
- Prefab 是 Asset 类型之一，扩展名为 `.prefab` 或 `.zgprefab`，内容保存 entity hierarchy 的可重建 JSON 数据。
- Prefab 文件读写属于 Runtime 序列化服务，不要求 VFS 已初始化。

## 测试要求

- AssetDatabase 扫描临时资源树。
- 常见扩展名分类：Texture、Mesh、Audio、World、Material、Script。
- Prefab 扩展名分类。
- Unknown 文件安全保留或按配置排除。
- 按 path、handle、type 查询。
- 路径排序稳定。
- Metadata 读写。
- Cache 命中。
- Invalid async load。
- 并发 async load 不创建重复 cache entry。
