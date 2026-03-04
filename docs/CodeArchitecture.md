# Zgine Engine - Code Architecture Documentation

## Overview

Zgine is a modern 3D game engine built with C++17, featuring an Entity Component System (ECS) architecture with a clean separation between the runtime core and editor tools. The engine follows industrial-grade design patterns including MVVM for editor UI and a low-abstraction, performance-focused approach for the runtime layer.

## Architecture Summary

```mermaid
graph TB
    subgraph "Application Layer"
        App[Application Entry]
        Editor[Editor Application]
        Sandbox[Sandbox Application]
    end

    subgraph "Editor Tools (MVVM Pattern)"
        EditorCore[Editor Core]
        Panels[UI Panels]
        Commands[Command System]
        ViewModels[ViewModels]
        EventBus[Event Bus]
    end

    subgraph "Engine Runtime (ECS + Procedural)"
        Scene[Scene System]
        ECS[Entity Component System]
        Systems[Runtime Systems]
    end

    subgraph "Core Systems"
        Renderer[Render System]
        Physics[Physics System]
        Audio[Audio System]
        Scripting[Script System]
    end

    subgraph "Foundation Layer"
        Core[Core Utilities]
        Platform[Platform Abstraction]
        Resources[Resource Management]
    end

    App --> Editor
    App --> Sandbox
    Editor --> EditorCore
    EditorCore --> Panels
    EditorCore --> Commands
    EditorCore --> ViewModels
    EditorCore --> EventBus

    Editor --> Scene
    Sandbox --> Scene
    Scene --> ECS
    ECS --> Systems
    Systems --> Renderer
    Systems --> Physics
    Systems --> Audio
    Systems --> Scripting

    Renderer --> Core
    Physics --> Core
    Audio --> Core
    Scripting --> Core
    Core --> Platform
    Core --> Resources
