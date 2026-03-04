#pragma once

/**
 * @file Components.h
 * @brief Unified header for all ECS components
 *
 * This header includes all component definitions organized by category.
 * Include this file to get access to all components in the Zgine ECS system.
 */

// Core components
#include <Zgine/Scene/Components/Core/TagComponent.h>
#include <Zgine/Scene/Components/Core/IDComponent.h>
#include <Zgine/Scene/Components/Core/RelationshipComponent.h>
#include <Zgine/Scene/Components/Core/TransformComponent.h>

// Rendering components
#include <Zgine/Scene/Components/Rendering/MeshComponent.h>
#include <Zgine/Scene/Components/Rendering/RenderComponents.h>
#include <Zgine/Scene/Components/Rendering/PBRMaterialComponent.h>
#include <Zgine/Scene/Components/Rendering/LightingComponents.h>

// Physics components
#include <Zgine/Scene/Components/Physics/PhysicsComponents.h>

// Audio components
#include <Zgine/Scene/Components/Audio/AudioComponents.h>

// Scripting components
#include <Zgine/Scene/Components/Scripting/ScriptComponent.h>

// Re-export PrimitiveType from mesh module
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
