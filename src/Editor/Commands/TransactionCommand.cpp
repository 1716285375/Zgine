#include <Zgine/Editor/Commands/TransactionCommand.h>

namespace Zgine {

TransactionCommand::TransactionCommand(const std::string& name, std::vector<std::unique_ptr<IEditorCommand>> commands)
    : m_Name(name)
    , m_Commands(std::move(commands))
{
}

TransactionCommand::~TransactionCommand() = default;

bool TransactionCommand::Execute() {
    // Commands already executed, this is just for redo
    for (auto& cmd : m_Commands) {
        if (!cmd->Execute()) {
            // Rollback
            for (int i = static_cast<int>(m_Commands.size()) - 1; i >= 0; --i) {
                m_Commands[i]->Undo();
            }
            return false;
        }
    }
    return true;
}

bool TransactionCommand::Undo() {
    // Undo in reverse order
    for (int i = static_cast<int>(m_Commands.size()) - 1; i >= 0; --i) {
        if (!m_Commands[i]->Undo()) {
            return false;
        }
    }
    return true;
}

std::string TransactionCommand::GetName() const {
    return m_Name;
}

} // namespace Zgine
