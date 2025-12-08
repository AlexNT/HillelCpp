#include <iostream>
#include <string>
#include <memory>
#include <queue>

class Document {
    std::string m_content;

public:
    Document() = default;

    Document(const std::string& text)
        : m_content(text)
    {
    }

    void insert(size_t pos, const std::string& str) {
        if (pos > m_content.size()) {
            pos = m_content.size();
        }
        m_content.insert(pos, str);
    }

    void erase(size_t pos, size_t count) {
        if (pos > m_content.size()) {
            return;
        }
        m_content.erase(pos, count);
    }

    void replace(size_t pos, size_t count, const std::string& str) {
        if (pos > m_content.size()) {
            return;
        }
        m_content.replace(pos, count, str);
    }

    const std::string& text() const {
        return m_content;
    }

    const std::string& getText() const {
        return m_content;
    }
};

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
};

class InsertTextCommand : public Command {
    std::weak_ptr<Document> m_doc;
    std::string m_text;
    size_t m_position;

public:
    InsertTextCommand(std::shared_ptr<Document> doc,
        std::string text,
        size_t position)
        : m_doc(doc), m_text(std::move(text)), m_position(position)
    {
    }

    void execute() override {
        std::shared_ptr<Document> doc = m_doc.lock();
        if (!doc) {
            std::cout << "Document no longer exists. Skipping.\n";
            return;
        }
        doc->insert(m_position, m_text);
    }
};

class EraseTextCommand : public Command {
    std::weak_ptr<Document> m_doc;
    size_t m_position;
    size_t m_count;

public:
    EraseTextCommand(std::shared_ptr<Document> doc,
        size_t position,
        size_t count)
        : m_doc(doc), m_position(position), m_count(count)
    {
    }

    void execute() override {
        std::shared_ptr<Document> doc = m_doc.lock();
        if (!doc) {
            std::cout << "Document no longer exists. Skipping.\n";
            return;
        }
        doc->erase(m_position, m_count);
    }
};

class ReplaceTextCommand : public Command {
    std::weak_ptr<Document> m_doc;
    std::string m_oldText;
    std::string m_newText;

public:
    ReplaceTextCommand(std::shared_ptr<Document> doc,
        std::string oldText,
        std::string newText)
        : m_doc(doc),
        m_oldText(std::move(oldText)),
        m_newText(std::move(newText))
    {
    }

    void execute() override {
        std::shared_ptr<Document> doc = m_doc.lock();
        if (!doc) {
            std::cout << "Document no longer exists. Skipping.\n";
            return;
        }
        const std::string& content = doc->text();
        size_t pos = content.find(m_oldText);
        if (pos == std::string::npos) {
            std::cout << "Substring not found. Skipping replace.\n";
            return;
        }
        doc->replace(pos, m_oldText.size(), m_newText);
    }
};

class CommandScheduler {
    std::queue<std::unique_ptr<Command>> m_pending;

public:
    void schedule(std::unique_ptr<Command> cmd) {
        m_pending.push(std::move(cmd));
    }

    void runAll() {
        while (!m_pending.empty()) {
            std::unique_ptr<Command> cmd = std::move(m_pending.front());
            m_pending.pop();
            if (cmd) {
                cmd->execute();
            }
        }
    }
};

int main() {
    auto doc1 = std::make_shared<Document>("Hello");
    auto doc2 = std::make_shared<Document>("World");

    CommandScheduler scheduler;

    std::cout << "Initial state:\n";
    std::cout << "doc1: \"" << doc1->getText() << "\"\n";
    std::cout << "doc2: \"" << doc2->getText() << "\"\n\n";

    scheduler.schedule(std::make_unique<InsertTextCommand>(doc1, " world", doc1->getText().size()));
    scheduler.schedule(std::make_unique<InsertTextCommand>(doc2, " framework", doc2->getText().size()));
    scheduler.schedule(std::make_unique<ReplaceTextCommand>(doc1, "world", "C++"));

    {
        auto tempDoc = std::make_shared<Document>("Temporary document");
        scheduler.schedule(std::make_unique<ReplaceTextCommand>(tempDoc, "Temporary", "ShouldNotAppear"));
    }

    std::cout << "Running all commands...\n\n";
    scheduler.runAll();

    std::cout << "Final state:\n";
    std::cout << "doc1: \"" << doc1->getText() << "\"\n";
    std::cout << "doc2: \"" << doc2->getText() << "\"\n";

    return 0;
}
