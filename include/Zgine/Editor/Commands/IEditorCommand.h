#pragma once

#include <string>

namespace Zgine {

/**
 * @brief Base interface for all editor commands
 *
 * Commands encapsulate actions that can be executed and undone.
 * This enables the undo/redo functionality in the editor.
 *
 * Design Pattern: Command Pattern
 *
 * Subclasses must implement:
 * - Execute(): Perform the action
 * - Undo(): Reverse the action
 * - GetName(): Return a description of the command
 *
 * Usage:
 * @code
 *   auto cmd = std::make_unique<CreateEntityCommand>(scene, "Cube");
 *   cmd->Execute();
 *   commandHistory.Push(std::move(cmd));
 *   // ... later ...
 *   commandHistory.Undo(); // Calls cmd->Undo()
 * @endcode
 */
class IEditorCommand {
public:
    virtual ~IEditorCommand() = default;

    /**
     * @brief Execute the command
     * @return true if successful, false otherwise
     */
    virtual bool Execute() = 0;

    /**
     * @brief Undo the command
     * @return true if successful, false otherwise
     */
    virtual bool Undo() = 0;

    /**
     * @brief Get human-readable name of the command
     */
    virtual std::string GetName() const = 0;

    /**
     * @brief Check if this command can be merged with another
     *
     * Some commands (like multiple transform adjustments) can be
     * merged into a single undo/redo step for better UX.
     *
     * @param other The command to potentially merge with
     * @return true if merge is possible
     */
    virtual bool CanMergeWith(const IEditorCommand* other) const {
        (void)other;
        return false;
    }

    /**
     * @brief Merge this command with another
     *
     * Called only if CanMergeWith() returned true.
     *
     * @param other The command to merge with
     */
    virtual void MergeWith(const IEditorCommand* other) {
        (void)other;
    }
};

} // namespace Zgine
