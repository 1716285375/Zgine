#pragma once

#include <Zgine/Editor/Commands/IEditorCommand.h>
#include <memory>
#include <vector>
#include <functional>

namespace Zgine {

/**
 * @brief Manages command history for undo/redo
 *
 * EditorCommandHistory maintains stacks of executed and undone
 * commands, enabling full undo/redo functionality with command
 * merging support.
 *
 * Features:
 * - Unlimited undo/redo (configurable max history size)
 * - Command merging for consecutive similar commands
 * - Transaction support (group multiple commands)
 * - Change notifications via callbacks
 */
class EditorCommandHistory {
public:
    EditorCommandHistory();
    ~EditorCommandHistory();

    /**
     * @brief Execute command and add to history
     * @param command Command to execute
     * @return true if command executed successfully
     */
    bool Execute(std::unique_ptr<IEditorCommand> command);

    /**
     * @brief Undo the last command
     * @return true if undo was successful
     */
    bool Undo();

    /**
     * @brief Redo the last undone command
     * @return true if redo was successful
     */
    bool Redo();

    /**
     * @brief Check if undo is possible
     */
    bool CanUndo() const;

    /**
     * @brief Check if redo is possible
     */
    bool CanRedo() const;

    /**
     * @brief Clear all history
     */
    void Clear();

    /**
     * @brief Get the name of the next undo command
     * @return Command name or empty string if can't undo
     */
    std::string GetUndoName() const;

    /**
     * @brief Get the name of the next redo command
     * @return Command name or empty string if can't redo
     */
    std::string GetRedoName() const;

    /**
     * @brief Set maximum history size
     * @param size Maximum number of commands to keep (0 = unlimited)
     */
    void SetMaxHistorySize(size_t size);

    /**
     * @brief Get current history size (undo stack size)
     */
    size_t GetHistorySize() const;

    /**
     * @brief Begin a transaction (group of commands)
     *
     * All commands executed between BeginTransaction() and
     * EndTransaction() will be treated as a single undo/redo step.
     *
     * @param name Name of the transaction
     */
    void BeginTransaction(const std::string& name);

    /**
     * @brief End the current transaction
     */
    void EndTransaction();

    /**
     * @brief Check if currently in a transaction
     */
    bool IsInTransaction() const;

    /**
     * @brief Set callback for when history changes
     *
     * Called whenever a command is executed, undone, or redone.
     * Useful for updating UI state (enabling/disabling undo/redo buttons)
     */
    using HistoryChangedCallback = std::function<void()>;
    void SetHistoryChangedCallback(HistoryChangedCallback callback);

private:
    void TrimHistory();
    void NotifyHistoryChanged();

    std::vector<std::unique_ptr<IEditorCommand>> m_UndoStack;
    std::vector<std::unique_ptr<IEditorCommand>> m_RedoStack;

    size_t m_MaxHistorySize = 0; // 0 = unlimited

    // Transaction support
    bool m_InTransaction = false;
    std::string m_TransactionName;
    std::vector<std::unique_ptr<IEditorCommand>> m_TransactionCommands;

    HistoryChangedCallback m_HistoryChangedCallback;
};

} // namespace Zgine
