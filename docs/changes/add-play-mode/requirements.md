# Requirements

## Functional Requirements

1. Editor 有明确 Play Mode 状态。
2. Enter Play 复制或构建 runtime World。
3. Runtime systems 通过统一入口启动、更新、停止。
4. Exit Play 丢弃 runtime World 并恢复 edit World。
5. Play Mode 状态变化发出 editor event。

## Non-Functional Requirements

1. Runtime 不依赖 Editor。
2. 系统更新顺序由 SystemManager 或 scene runtime coordinator 控制。
3. 启停过程必须幂等。
4. 至少有状态切换和 world 隔离测试。
