# Proposal: Add Asset Database

## 背景

Zgine 已有 AssetManager、metadata、importer、cache 和 hot reload 基础，但缺少一个面向编辑器和工具的 AssetDatabase。没有 AssetDatabase，Editor Asset Browser、Prefab、Scene 引用和资源搜索都会变成临时扫描或直接路径访问。

## 目标

- 建立 Runtime 可用的 AssetDatabase 服务。
- 扫描 `assets/` 并生成稳定的 asset metadata 列表。
- 支持按 handle/path/type 查询。
- 为 Editor Asset Browser 和 Inspector 提供数据源。

## 非目标

- 本次不做完整导入管线。
- 本次不做缩略图缓存。
- 本次不做资源拖拽生成实体。
- 本次不做远程资源或包体系统。

## 风险

- File/VFS/absolute path 语义不统一会导致跨平台问题。
- 后台扫描和 AssetManager cache 同步需要清晰线程边界。
