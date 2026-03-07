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
 *   auto cmd = std::make_unique<CreateEntityCommand>(World, "Cube");
 *   cmd->Execute();
 *   commandHistory.Push(std::move(cmd));
 *   // ... later ...
 *   commandHistory.Undo(); // Calls cmd->Undo()
 * @endcode
 */
class IEditorCommand {
public:
    virtual ~IEditorCommand() = default;

    /*
        Purpose : Execute the command action.
        Return  : true if the action succeeded.
    */
    [[nodiscard]] virtual bool Execute() = 0;

    /*
        Purpose : Reverse the previously executed action.
        Return  : true if undo succeeded.
    */
    [[nodiscard]] virtual bool Undo() = 0;

    /*
        Purpose : Return a short human-readable description shown in the undo history UI.
    */
    [[nodiscard]] virtual std::string GetName() const = 0;

    /*
        Purpose : Check if this command can be merged with another to form one undo step.
        Notes   : E.g. consecutive transforms merge into a single undo entry.
        Args    : other — candidate command for merging.
        Return  : true if merge is possible.
    */
    [[nodiscard]] virtual bool CanMergeWith(const IEditorCommand* other) const {
        (void)other;
        return false;
    }

    /*
        Purpose : Absorb another command's data into this one; only called after CanMergeWith returns true.
        Args    : other — command to merge into this.
    */
    virtual void MergeWith(const IEditorCommand* other) {
        (void)other;
    }
};

} // namespace Zgine
