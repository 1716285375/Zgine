# Spec: Audio

版本日期：2026-05-25

## 职责

Audio 使用 miniaudio 解释 audio components，维护 listener/source runtime state，控制播放、停止、空间化参数。

## 不负责

- 不保存 miniaudio 指针到场景文件。
- 不直接访问 Editor。
- 不负责资源导入。

## 规则

- Component 保存播放配置和 asset handle/path。
- Runtime voice/source 对象由 AudioSystem 管理。
- `OnSceneStart/OnSceneStop` 负责 source 创建和销毁；`Update` 负责 listener/source 位置同步。
- Play Mode 中 AudioSystem 由 runtime World 的 `SystemManager::UpdateAll` 调度，Editor 不直接逐帧调用。
- Runtime clone 不得继承 miniaudio source 指针或播放中状态；进入 Play 后由 AudioSystem 为 runtime World 重建 source。
- 空 marker component 不适合需要 uniform `GetComponent<T>() -> T&` 的 ECS API；需要可引用字段。

## 测试要求

- 组件序列化。
- listener/source 默认值。
- 无音频设备环境下系统应能安全失败或跳过。
