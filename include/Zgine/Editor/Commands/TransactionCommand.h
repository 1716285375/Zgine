#pragma once

#include <Zgine/Editor/Commands/IEditorCommand.h>
#include <memory>
#include <vector>

namespace Zgine {

/**
 * @brief Composite command that groups multiple commands into a transaction
 *
 * Allows multiple commands to be executed/undone as a single operation
 */
class TransactionCommand : public IEditorCommand {
public:
    TransactionCommand(const std::string& name, std::vector<std::unique_ptr<IEditorCommand>> commands);
    ~TransactionCommand() override;

    // EditorCommand interface
    bool Execute() override;
    bool Undo() override;
    std::string GetName() const override;

private:
    std::string m_Name;
    std::vector<std::unique_ptr<IEditorCommand>> m_Commands;
};

} // namespace Zgine
