#pragma once

/**
 * @file Components.h
 * @brief Unified header for all ECS components
 *
 * This header includes all component definitions organized by category.
 * Include this file to get access to all components in the Zgine ECS system.
 */

// Core components
#include <Zgine/World/Components/Core/TagComponent.h>
#include <Zgine/World/Components/Core/IDComponent.h>
#include <Zgine/World/Components/Core/RelationshipComponent.h>
#include <Zgine/World/Components/Core/TransformComponent.h>

// Rendering components
#include <Zgine/World/Components/Rendering/MeshComponent.h>
#include <Zgine/World/Components/Rendering/RenderComponents.h>
#include <Zgine/World/Components/Rendering/PBRMaterialComponent.h>
#include <Zgine/World/Components/Rendering/LightingComponents.h>

// Physics components
#include <Zgine/World/Components/Physics/PhysicsComponents.h>

// Audio components
#include <Zgine/World/Components/Audio/AudioComponents.h>

// Scripting components
#include <Zgine/World/Components/Scripting/ScriptComponent.h>

// Re-export PrimitiveType from mesh module
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
