#ifndef REARK_MARKDOWN_RENDERER_H
#define REARK_MARKDOWN_RENDERER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QVector>

struct MarkdownCodeBlock {
    QString token;
    QString language;
    QString code;
};

class MarkdownRenderer : public QObject {
    Q_OBJECT

public:
    explicit MarkdownRenderer(QObject* parent = nullptr);

    Q_INVOKABLE QString render(const QString& markdown, bool darkTheme) const;

private:
    [[nodiscard]] QString prepareMarkdown(const QString& markdown, QVector<MarkdownCodeBlock>* codeBlocks) const;
    [[nodiscard]] QString renderCodeBlock(const MarkdownCodeBlock& block, bool darkTheme) const;
    [[nodiscard]] QString highlightCode(const QString& code, const QString& language, bool darkTheme) const;
    [[nodiscard]] QString styleSheet(bool darkTheme) const;
    void rememberRenderedHtml(const QString& key, const QString& html) const;

    mutable QHash<QString, QString> cache_;
    mutable QStringList cacheOrder_;
};

#endif // REARK_MARKDOWN_RENDERER_H
