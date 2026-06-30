# Spec: Core

版本日期：2026-05-29

## 职责

Core 提供引擎基础设施：Application、Layer、Event、Log、Assert、Time、Timer、Job、Memory、UUID、Math 抽象。

## 不负责

- 不拥有 World。
- 不调用 Renderer。
- 不知道 Editor、Physics、Audio、Scripting 存在。
- 不处理资源导入和场景保存。

## 规则

- Core public API 尽量只依赖标准库和轻量基础类型。
- Core 类型必须稳定，避免频繁破坏上层接口。
- 低层代码不能假设日志系统一定初始化；`Log::GetCoreLogger()` 和 `Log::GetClientLogger()` 必须提供安全 fallback。
- Math public API 使用 `Zgine::Math::*`，不直接向上暴露 GLM。

## 测试要求

- UUID、Time、Math、Event 分发、Application 基础生命周期应有最小测试或 compile smoke。
- Core 改动不能要求 Editor 或 Renderer 初始化。
- 日志宏在 `Log::Init()` 前调用时不得崩溃。
