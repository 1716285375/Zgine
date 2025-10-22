# Contributing to Zgine

Thank you for your interest in contributing to Zgine! This document provides guidelines and information for contributors.

## 🚀 Getting Started

### Prerequisites
- Visual Studio 2022 (Windows)
- Git
- Basic knowledge of C++17 and OpenGL

### Development Setup
1. Fork the repository
2. Clone your fork locally
3. Generate project files using premake5
4. Build the project
5. Create a feature branch

## 📝 Code Style

### C++ Guidelines
- Use C++17 features
- Follow RAII principles
- Use smart pointers (`std::shared_ptr`, `std::unique_ptr`)
- Prefer `const` correctness
- Use meaningful variable and function names

### Naming Conventions
- Classes: `PascalCase` (e.g., `BatchRenderer2D`)
- Functions: `PascalCase` (e.g., `DrawQuad`)
- Variables: `camelCase` (e.g., `m_CameraPosition`)
- Constants: `UPPER_CASE` (e.g., `MAX_QUADS`)

### File Organization
- One class per header/source file pair
- Use forward declarations when possible
- Group related functionality in namespaces

## 🐛 Bug Reports

When reporting bugs, please include:
- Description of the issue
- Steps to reproduce
- Expected vs actual behavior
- System information (OS, compiler version)
- Screenshots if applicable

## ✨ Feature Requests

For new features:
- Check existing issues first
- Provide a clear description
- Explain the use case and benefits
- Consider implementation complexity

## 🔧 Pull Request Process

1. **Create a feature branch** from `main`
2. **Make your changes** following the code style
3. **Test your changes** thoroughly
4. **Update documentation** if needed
5. **Submit a pull request** with a clear description

### PR Guidelines
- Keep PRs focused and reasonably sized
- Include tests for new functionality
- Update README if adding new features
- Ensure all tests pass

## 📋 Development Roadmap

### Current Focus Areas
- Enhanced 2D rendering (lines, circles)
- Resource management system
- Entity Component System (ECS)
- 3D rendering support

### Long-term Goals
- Cross-platform support
- Physics integration
- Scripting system
- Editor tools

## 🤝 Community Guidelines

- Be respectful and inclusive
- Help others learn and grow
- Provide constructive feedback
- Follow the code of conduct

## 📞 Getting Help

- **Issues**: Use GitHub Issues for bugs and feature requests
- **Discussions**: Use GitHub Discussions for questions and ideas
- **Documentation**: Check the Wiki for detailed guides

Thank you for contributing to Zgine! 🎮
