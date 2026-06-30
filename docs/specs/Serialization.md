# Spec: Serialization

版本日期：2026-05-25

## 职责

Serialization 负责 World、Entity、Component 和场景文件的保存/加载。

## 不负责

- 不保存 backend native object。
- 不执行 Editor-specific command。
- 不触发渲染、物理或音频 runtime 创建，除非通过明确加载流程。

## 规则

- JSON 字段名保持稳定。
- 新字段必须有默认值。
- 删除字段需要迁移说明。
- 序列化只保存可重建数据。

## 测试要求

- Round-trip。
- 缺失字段。
- 未知字段容忍。
- 父子关系和 UUID 恢复。
