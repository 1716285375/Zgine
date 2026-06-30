# Proposal: Add Prefab System

## 背景

Zgine 目前可以创建实体和组件，但缺少可复用游戏对象模板。没有 Prefab，游戏对象复用、编辑器拖拽创建、场景实例同步和教学演示都会受限。

## 目标

- 定义 Prefab asset。
- 支持从 World entity hierarchy 创建 Prefab。
- 支持实例化 Prefab 到 World。
- 支持基础 override 记录或预留扩展点。
- 支持序列化 Prefab。

## 非目标

- 本次不做 Unity 级嵌套 prefab variant 完整系统。
- 本次不做复杂 override diff UI。
- 本次不做多人协作 merge 工具。

## 风险

- Prefab 涉及 World、Serialization、Asset、Editor、Undo/Redo，多模块耦合风险高。
- Runtime handle 和 UUID 处理不当会导致实例关系混乱。
