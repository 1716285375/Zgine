# Zgine Engine - 编译速度优化指南

## 概述
本文档提供了Zgine引擎的编译速度优化方案，包括多种编译脚本和配置优化。

## 编译脚本说明

### 1. FastBuild.bat - 快速编译脚本
- **用途**: 完整的项目编译
- **特点**: 使用8个并行进程，优化链接选项
- **用法**: 
  - `FastBuild.bat` - Debug模式
  - `FastBuild.bat release` - Release模式
  - `FastBuild.bat dist` - Dist模式
  - `FastBuild.bat clean` - 清理后编译

### 2. IncrementalBuild.bat - 增量编译脚本
- **用途**: 只编译修改过的文件
- **特点**: 使用共享编译，增量构建
- **用法**: 
  - `IncrementalBuild.bat` - Debug模式
  - `IncrementalBuild.bat release` - Release模式

### 3. DevBuild.bat - 开发模式编译脚本
- **用途**: 只编译Sandbox项目
- **特点**: 最快的编译速度，适合开发调试
- **用法**: `DevBuild.bat`

### 4. BuildSpeedTest.bat - 编译速度测试脚本
- **用途**: 测试编译速度
- **特点**: 清理项目后重新编译，记录编译时间
- **用法**: `BuildSpeedTest.bat`

## 编译优化配置

### 1. 多处理器编译
```lua
flags { "MultiProcessorCompile" }
buildoptions { "/MP", "/bigobj" }
```

### 2. 预编译头文件
```lua
pchheader "zgpch.h"
pchsource "Zgine/src/zgpch.cpp"
```

### 3. 链接优化
```lua
-- Debug模式
linkoptions { "/DEBUG" }

-- Release模式
linkoptions { "/LTCG", "/OPT:REF", "/OPT:ICF" }
```

### 4. 编译器优化
```lua
-- Debug模式
buildoptions { "/MP", "/bigobj", "/Zi" }

-- Release模式
buildoptions { "/MP", "/O2", "/bigobj", "/GL" }
```

## 性能优化建议

### 1. 硬件优化
- **CPU**: 使用多核CPU（推荐8核以上）
- **内存**: 至少16GB RAM
- **存储**: 使用SSD存储项目文件

### 2. 开发环境优化
- **Visual Studio**: 使用最新版本的Visual Studio
- **MSBuild**: 确保使用最新版本的MSBuild
- **Windows**: 使用Windows 10/11

### 3. 项目结构优化
- **预编译头**: 合理使用预编译头文件
- **模块化**: 将大型项目分解为多个模块
- **依赖管理**: 减少不必要的依赖

### 4. 编译选项优化
- **并行编译**: 使用`/MP`选项启用多处理器编译
- **大对象支持**: 使用`/bigobj`选项支持大型对象文件
- **增量编译**: 使用`/Zi`选项生成调试信息
- **全程序优化**: 使用`/GL`选项启用全程序优化

## 编译时间对比

| 编译模式 | 预计时间 | 说明 |
|---------|---------|------|
| 完整编译 | 2-5分钟 | 首次编译或清理后编译 |
| 增量编译 | 10-30秒 | 修改少量文件后编译 |
| 开发编译 | 5-15秒 | 只编译Sandbox项目 |

## 故障排除

### 1. 编译失败
- 检查Visual Studio版本
- 检查MSBuild路径
- 检查项目文件是否正确生成

### 2. 编译速度慢
- 检查CPU核心数
- 检查内存使用情况
- 检查磁盘空间

### 3. 链接错误
- 检查库文件路径
- 检查依赖关系
- 检查链接选项

## 最佳实践

### 1. 开发流程
1. 使用`DevBuild.bat`进行快速开发
2. 使用`IncrementalBuild.bat`进行增量编译
3. 使用`FastBuild.bat`进行完整编译

### 2. 性能监控
1. 使用`BuildSpeedTest.bat`测试编译速度
2. 监控编译时间变化
3. 优化慢速编译的文件

### 3. 维护建议
1. 定期清理项目文件
2. 更新编译工具链
3. 优化项目结构

## 总结

通过使用这些编译脚本和优化配置，可以显著提高Zgine引擎的编译速度。建议根据开发需求选择合适的编译脚本，并定期测试编译性能。
