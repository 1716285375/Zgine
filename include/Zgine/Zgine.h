#pragma once

// 为 Zgine 应用程序提供的头文件

#include <Zgine/Core/Base/Prerequisites.h>
#include <Zgine/Core/Application/Application.h>
#include <Zgine/Core/Layers/Layer.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Base/Assert.h>
#include <Zgine/Core/Time/Timestep.h>
#include <Zgine/Gui/GuiLayer.h>

#include <Zgine/Platform/Input.h>
#include <Zgine/Platform/Window.h>

#include <Zgine/Renderer/Pipeline/RenderSystem.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Renderer/RHI/VertexBuffer.h>
#include <Zgine/Renderer/RHI/IndexBuffer.h>
#include <Zgine/Renderer/RHI/VertexArray.h>
#include <Zgine/Renderer/RHI/Shader.h>
#include <Zgine/Renderer/RHI/Texture.h>

#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Camera/Camera.h>
#include <Zgine/World/Serialization/WorldSerializer.h>

#include <Zgine/Physics/PhysicsSystem.h>
#include <Zgine/Audio/AudioSystem.h>
#include <Zgine/Scripting/ScriptSystem.h>
#include <Zgine/Resources/Core/AssetDatabase.h>
#include <Zgine/Resources/Core/AssetManager.h>
#include <Zgine/Resources/Material/PBRMaterialPreset.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <Zgine/Resources/Prefab/PrefabAsset.h>
#include <Zgine/Resources/Prefab/PrefabSerializer.h>

#include <Zgine/Runtime/SceneRuntime.h>

#include <Zgine/Platform/IO/File.h>
#include <Zgine/Platform/IO/VFS.h>
#include <Zgine/Core/UUID/UUID.h>

#include <Zgine/Core/Events/Event.h>
#include <Zgine/Core/Events/ApplicationEvent.h>
#include <Zgine/Core/Events/KeyEvent.h>
#include <Zgine/Core/Events/MouseEvent.h>
