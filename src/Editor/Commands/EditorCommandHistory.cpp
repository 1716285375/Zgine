#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/TransactionCommand.h>
#include <Zgine/Core/Log/Log.h>

namespace Zgine {

EditorCommandHistory::EditorCommandHistory()
    : m_MaxHistorySize(0)
    , m_InTransaction(false)
{
}

EditorCommandHistory::~EditorCommandHistory() {
    Clear();
}

bool EditorCommandHistory::Execute(std::unique_ptr<IEditorCommand> command) {
    if (!command) {
        if (auto& logger = Log::GetCoreLogger()) {
            logger->error("EditorCommandHistory::Execute() called with null command");
        }
        return false;
    }

    // Execute the command
    if (!command->Execute()) {
        if (auto& logger = Log::GetCoreLogger()) {
            logger->error("Command failed to execute: {}", command->GetName());
        }
        return false;
    }

    // If in transaction, add to transaction list
    if (m_InTransaction) {
        m_TransactionCommands.push_back(std::move(command));
        return true;
    }

    // Try to merge with last command
    if (!m_UndoStack.empty()) {
        IEditorCommand* lastCmd = m_UndoStack.back().get();
        if (lastCmd->CanMergeWith(command.get())) {
            lastCmd->MergeWith(command.get());
            NotifyHistoryChanged();
            return true;
        }
    }

    // Add to undo stack
    m_UndoStack.push_back(std::move(command));

    // Clear redo stack (new command invalidates redo history)
    m_RedoStack.clear();

    // Trim if needed
    TrimHistory();

    NotifyHistoryChanged();
    return true;
}

bool EditorCommandHistory::Undo() {
    if (!CanUndo()) {
        return false;
    }

    auto& command = m_UndoStack.back();
    if (!command->Undo()) {
        if (auto& logger = Log::GetCoreLogger()) {
            logger->error("Command failed to undo: {}", command->GetName());
        }
        return false;
    }

    // Move to redo stack
    m_RedoStack.push_back(std::move(command));
    m_UndoStack.pop_back();

    NotifyHistoryChanged();
    return true;
}

bool EditorCommandHistory::Redo() {
    if (!CanRedo()) {
        return false;
    }

    auto& command = m_RedoStack.back();
    if (!command->Execute()) {
        if (auto& logger = Log::GetCoreLogger()) {
            logger->error("Command failed to redo: {}", command->GetName());
        }
        return false;
    }

    // Move back to undo stack
    m_UndoStack.push_back(std::move(command));
    m_RedoStack.pop_back();

    NotifyHistoryChanged();
    return true;
}

bool EditorCommandHistory::CanUndo() const {
    return !m_UndoStack.empty() && !m_InTransaction;
}

bool EditorCommandHistory::CanRedo() const {
    return !m_RedoStack.empty() && !m_InTransaction;
}

void EditorCommandHistory::Clear() {
    m_UndoStack.clear();
    m_RedoStack.clear();
    m_TransactionCommands.clear();
    m_InTransaction = false;
    NotifyHistoryChanged();
}

std::string EditorCommandHistory::GetUndoName() const {
    if (m_UndoStack.empty()) {
        return "";
    }
    return m_UndoStack.back()->GetName();
}

std::string EditorCommandHistory::GetRedoName() const {
    if (m_RedoStack.empty()) {
        return "";
    }
    return m_RedoStack.back()->GetName();
}

void EditorCommandHistory::SetMaxHistorySize(size_t size) {
    m_MaxHistorySize = size;
    TrimHistory();
}

size_t EditorCommandHistory::GetHistorySize() const {
    return m_UndoStack.size();
}

void EditorCommandHistory::BeginTransaction(const std::string& name) {
    if (m_InTransaction) {
        if (auto& logger = Log::GetCoreLogger()) {
            logger->warn("EditorCommandHistory::BeginTransaction() called while already in transaction");
        }
        return;
    }

    m_InTransaction = true;
    m_TransactionName = name;
    m_TransactionCommands.clear();
}

void EditorCommandHistory::EndTransaction() {
    if (!m_InTransaction) {
        return; // Silently ignore if not in transaction
    }

    m_InTransaction = false;

    // If no commands were executed in transaction, just return
    if (m_TransactionCommands.empty()) {
        return;
    }

    // Create a composite transaction command
    auto transactionCmd = std::make_unique<TransactionCommand>(
        m_TransactionName,
        std::move(m_TransactionCommands)
    );

    m_TransactionCommands.clear();

    // Add transaction as single undo step
    m_UndoStack.push_back(std::move(transactionCmd));

    // Clear redo stack
    m_RedoStack.clear();

    TrimHistory();
    NotifyHistoryChanged();
}

bool EditorCommandHistory::IsInTransaction() const {
    return m_InTransaction;
}

void EditorCommandHistory::SetHistoryChangedCallback(HistoryChangedCallback callback) {
    m_HistoryChangedCallback = std::move(callback);
}

void EditorCommandHistory::TrimHistory() {
    if (m_MaxHistorySize == 0) {
        return; // Unlimited history
    }

    while (m_UndoStack.size() > m_MaxHistorySize) {
        m_UndoStack.erase(m_UndoStack.begin());
    }
}

void EditorCommandHistory::NotifyHistoryChanged() {
    if (m_HistoryChangedCallback) {
        m_HistoryChangedCallback();
    }
}

} // namespace Zgine
