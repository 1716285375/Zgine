# Proposal: Add Play Mode

## 背景

Zgine 需要从“能编辑场景”走向“能制作游戏”。Play Mode 是编辑器和运行时之间的关键桥梁：用户在编辑器中启动场景，Runtime 系统按游戏生命周期运行，停止后恢复编辑状态。

## 目标

- 定义 Edit Mode / Play Mode 状态。
- Play Mode 启动时创建运行时 World 副本。
- Physics、Audio、Scripting、Renderer 使用统一生命周期。
- Stop 后回到编辑 World。
- Editor UI 能显示当前模式。

## 非目标

- 本次不做热重载脚本完整方案。
- 本次不做多场景 additive play。
- 本次不做打包发布流程。

## 风险

- 直接在编辑 World 上运行会污染编辑状态。
- 系统生命周期分散会导致启动/停止顺序不可靠。
