#include "presentation/MarkdownRenderer.h"

#include "presentation/CodeTheme.h"

#include <QColor>
#include <QFontDatabase>
#include <QRegularExpression>
#include <QTextDocument>

namespace {

bool isFenceLine(const QString& line, QString* marker, QString* info)
{
    const QString trimmed = line.trimmed();
    if (trimmed.size() < 3) {
        return false;
    }

    const QChar fenceChar = trimmed.at(0);
    if (fenceChar != QLatin1Char('`') && fenceChar != QLatin1Char('~')) {
        return false;
    }

    int count = 0;
    while (count < trimmed.size() && trimmed.at(count) == fenceChar) {
        ++count;
    }
    if (count < 3) {
        return false;
    }

    *marker = QString(count, fenceChar);
    *info = trimmed.mid(count).trimmed();
    return true;
}

QString normalizedLanguage(QString language)
{
    language = language.trimmed().toLower();
    const int separator = language.indexOf(QRegularExpression(QStringLiteral("[\\s,{]")));
    if (separator > 0) {
        language.truncate(separator);
    }

    if (language == QStringLiteral("typescript") || language == QStringLiteral("arkts")) {
        return QStringLiteral("ts");
    }
    if (language == QStringLiteral("javascript")) {
        return QStringLiteral("js");
    }
    if (language == QStringLiteral("cpp") || language == QStringLiteral("c++")) {
        return QStringLiteral("cpp");
    }
    return language;
}

QString displayLanguage(const QString& language)
{
    const QString normalized = normalizedLanguage(language);
    if (normalized == QStringLiteral("text") || normalized == QStringLiteral("plain")) {
        return {};
    }
    if (normalized == QStringLiteral("ets")) {
        return QStringLiteral("ETS");
    }
    if (normalized == QStringLiteral("ts")) {
        return QStringLiteral("TypeScript");
    }
    if (normalized == QStringLiteral("js")) {
        return QStringLiteral("JavaScript");
    }
    if (normalized == QStringLiteral("json")) {
        return QStringLiteral("JSON");
    }
    if (normalized == QStringLiteral("cpp")) {
        return QStringLiteral("C++");
    }
    if (normalized.isEmpty()) {
        return QStringLiteral("Code");
    }
    QString label = normalized;
    label[0] = label.at(0).toUpper();
    return label;
}

bool isPlainTextLanguage(const QString& language)
{
    const QString normalized = normalizedLanguage(language);
    return normalized.isEmpty()
        || normalized == QStringLiteral("text")
        || normalized == QStringLiteral("plain")
        || normalized == QStringLiteral("plaintext")
        || normalized == QStringLiteral("txt")
        || normalized == QStringLiteral("console");
}

bool isIdentifierStart(QChar ch)
{
    return ch.isLetter() || ch == QLatin1Char('_') || ch == QLatin1Char('$');
}

bool isIdentifierPart(QChar ch)
{
    return ch.isLetterOrNumber() || ch == QLatin1Char('_') || ch == QLatin1Char('$');
}

bool isKeyword(QStringView token)
{
    static const QStringList keywords {
        QStringLiteral("abstract"), QStringLiteral("as"), QStringLiteral("async"),
        QStringLiteral("await"), QStringLiteral("break"), QStringLiteral("case"),
        QStringLiteral("catch"), QStringLiteral("class"), QStringLiteral("const"),
        QStringLiteral("continue"), QStringLiteral("default"), QStringLiteral("delete"),
        QStringLiteral("do"), QStringLiteral("else"), QStringLiteral("enum"),
        QStringLiteral("export"), QStringLiteral("extends"), QStringLiteral("false"),
        QStringLiteral("finally"), QStringLiteral("for"), QStringLiteral("from"),
        QStringLiteral("function"), QStringLiteral("if"), QStringLiteral("import"),
        QStringLiteral("in"), QStringLiteral("instanceof"), QStringLiteral("interface"),
        QStringLiteral("let"), QStringLiteral("namespace"), QStringLiteral("new"),
        QStringLiteral("nullptr"), QStringLiteral("null"), QStringLiteral("private"),
        QStringLiteral("protected"), QStringLiteral("public"), QStringLiteral("return"),
        QStringLiteral("static"), QStringLiteral("struct"), QStringLiteral("super"),
        QStringLiteral("switch"), QStringLiteral("this"), QStringLiteral("throw"),
        QStringLiteral("true"), QStringLiteral("try"), QStringLiteral("typedef"),
        QStringLiteral("typename"), QStringLiteral("typeof"), QStringLiteral("undefined"),
        QStringLiteral("using"), QStringLiteral("var"), QStringLiteral("void"),
        QStringLiteral("while"), QStringLiteral("yield")
    };
    return keywords.contains(token.toString());
}

bool isTypeName(QStringView token)
{
    static const QStringList types {
        QStringLiteral("Array"), QStringLiteral("Boolean"), QStringLiteral("Map"),
        QStringLiteral("Number"), QStringLiteral("Object"), QStringLiteral("Promise"),
        QStringLiteral("Record"), QStringLiteral("Set"), QStringLiteral("String"),
        QStringLiteral("auto"), QStringLiteral("any"), QStringLiteral("bigint"),
        QStringLiteral("bool"), QStringLiteral("boolean"), QStringLiteral("char"),
        QStringLiteral("double"), QStringLiteral("float"), QStringLiteral("int"),
        QStringLiteral("long"), QStringLiteral("never"), QStringLiteral("number"),
        QStringLiteral("object"), QStringLiteral("short"), QStringLiteral("size_t"),
        QStringLiteral("std"), QStringLiteral("string"), QStringLiteral("symbol"),
        QStringLiteral("unknown")
    };
    return types.contains(token.toString());
}

QString span(const QString& text, const QColor& color)
{
    if (text.isEmpty()) {
        return {};
    }
    return QStringLiteral("<span style=\"color:%1;\">%2</span>")
        .arg(color.name(QColor::HexRgb), text.toHtmlEscaped());
}

QString escapedPlain(QStringView text, int start, int length)
{
    return text.mid(start, length).toString().toHtmlEscaped();
}

QString htmlColor(const QColor& color)
{
    return color.name(QColor::HexRgb);
}

QString monoFontFamily()
{
    const QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    QString family = font.family();
    if (family.isEmpty()) {
        family = QStringLiteral("Consolas");
    }
    return family.replace(QLatin1Char('\''), QStringLiteral("\\'"));
}

QString htmlTableCodeBlock(
    const QString& languageLabel,
    const QString& highlightedCode,
    const QString& plainCode,
    bool darkTheme,
    const CodeTheme& theme)
{
    const QString border = darkTheme ? QStringLiteral("#2b394b") : QStringLiteral("#d0d7de");
    const QString headerBackground = darkTheme ? QStringLiteral("#111a26") : QStringLiteral("#f6f8fa");
    const QString headerText = darkTheme ? QStringLiteral("#9fb0c4") : QStringLiteral("#57606a");
    const QString codeBackground = darkTheme ? QStringLiteral("#0b111a") : QStringLiteral("#ffffff");
    const QString codeText = htmlColor(theme.text);
    const QString fontFamily = monoFontFamily();
    const QString headerRow = languageLabel.isEmpty()
        ? QString()
        : QStringLiteral(
              "<tr><td bgcolor=\"%1\" style=\"padding:6px 10px; border-bottom:1px solid %2; "
              "color:%3; font-size:11px; font-family:'Segoe UI', sans-serif;\">%4</td></tr>")
              .arg(headerBackground,
                   border,
                   headerText,
                   languageLabel.toHtmlEscaped());

    return QStringLiteral(
        "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" "
        "style=\"margin-top:12px; margin-bottom:14px; border:1px solid %1; border-collapse:collapse;\">"
        "%2"
        "<tr><td bgcolor=\"%3\" style=\"padding:10px 12px; color:%4; "
        "font-family:'%6', Consolas, monospace; font-size:12px; line-height:1.48; "
        "white-space:pre-wrap;\">%5</td></tr>"
        "</table>")
        .arg(border,
             headerRow,
             codeBackground,
             codeText,
             highlightedCode.isEmpty() && !plainCode.isEmpty() ? QStringLiteral("&nbsp;") : highlightedCode,
             fontFamily);
}

QString highlightedLine(QStringView text, bool* inBlockComment, const CodeTheme& theme)
{
    QString html;
    html.reserve(text.size() * 2);

    int i = 0;
    while (i < text.size()) {
        if (*inBlockComment) {
            const int start = i;
            while (i + 1 < text.size()
                && !(text.at(i) == QLatin1Char('*') && text.at(i + 1) == QLatin1Char('/'))) {
                ++i;
            }
            if (i + 1 >= text.size()) {
                html += span(text.mid(start).toString(), theme.comment);
                return html;
            }
            i += 2;
            html += span(text.mid(start, i - start).toString(), theme.comment);
            *inBlockComment = false;
            continue;
        }

        const QChar ch = text.at(i);
        if (ch == QLatin1Char('/') && i + 1 < text.size()) {
            const QChar next = text.at(i + 1);
            if (next == QLatin1Char('/')) {
                html += span(text.mid(i).toString(), theme.comment);
                return html;
            }
            if (next == QLatin1Char('*')) {
                *inBlockComment = true;
                continue;
            }
        }

        if (ch == QLatin1Char('"') || ch == QLatin1Char('\'') || ch == QLatin1Char('`')) {
            const QChar quote = ch;
            const int start = i++;
            bool escaped = false;
            while (i < text.size()) {
                const QChar current = text.at(i++);
                if (escaped) {
                    escaped = false;
                } else if (current == QLatin1Char('\\')) {
                    escaped = true;
                } else if (current == quote) {
                    break;
                }
            }
            html += span(text.mid(start, i - start).toString(), theme.string);
            continue;
        }

        if (ch.isDigit()) {
            const int start = i++;
            while (i < text.size()
                && (text.at(i).isLetterOrNumber() || text.at(i) == QLatin1Char('.')
                    || text.at(i) == QLatin1Char('_'))) {
                ++i;
            }
            html += span(text.mid(start, i - start).toString(), theme.number);
            continue;
        }

        if (isIdentifierStart(ch)) {
            const int start = i++;
            while (i < text.size() && isIdentifierPart(text.at(i))) {
                ++i;
            }
            const QStringView token = text.mid(start, i - start);
            if (isKeyword(token)) {
                html += span(token.toString(), theme.keyword);
            } else if (isTypeName(token)) {
                html += span(token.toString(), theme.type);
            } else {
                html += escapedPlain(text, start, i - start);
            }
            continue;
        }

        html += QString(ch).toHtmlEscaped();
        ++i;
    }

    return html;
}

} // namespace

MarkdownRenderer::MarkdownRenderer(QObject* parent)
    : QObject(parent)
{
}

QString MarkdownRenderer::render(const QString& markdown, bool darkTheme) const
{
    static constexpr qsizetype kMaxCacheEntries = 96;

    QString key;
    key.reserve(markdown.size() + 2);
    key.append(darkTheme ? QLatin1Char('1') : QLatin1Char('0'));
    key.append(QChar(0x1f));
    key.append(markdown);
    const auto cached = cache_.constFind(key);
    if (cached != cache_.constEnd()) {
        return cached.value();
    }

    QVector<MarkdownCodeBlock> codeBlocks;
    const QString prepared = prepareMarkdown(markdown, &codeBlocks);
    if (prepared.trimmed().isEmpty() && codeBlocks.isEmpty()) {
        return {};
    }

    QTextDocument document;
    document.setDocumentMargin(0);
    document.setDefaultStyleSheet(styleSheet(darkTheme));
    QTextDocument::MarkdownFeatures features = QTextDocument::MarkdownDialectGitHub;
    features.setFlag(QTextDocument::MarkdownNoHTML);
    document.setMarkdown(prepared, features);
    QString html = document.toHtml();
    for (const MarkdownCodeBlock& block : codeBlocks) {
        const QString replacement = renderCodeBlock(block, darkTheme);
        const QRegularExpression paragraphToken(QStringLiteral("<p[^>]*>\\s*%1\\s*</p>")
                                                    .arg(QRegularExpression::escape(block.token)));
        html.replace(paragraphToken, replacement);
        html.replace(block.token, replacement);
    }
    rememberRenderedHtml(key, html);
    while (cacheOrder_.size() > kMaxCacheEntries) {
        cache_.remove(cacheOrder_.takeFirst());
    }
    return html;
}

QString MarkdownRenderer::prepareMarkdown(const QString& markdown, QVector<MarkdownCodeBlock>* codeBlocks) const
{
    QString output;
    output.reserve(markdown.size());

    const QStringList lines = markdown.split(QLatin1Char('\n'));
    bool inFence = false;
    QString fenceMarker;
    QString language;
    QStringList codeLines;

    for (const QString& line : lines) {
        QString marker;
        QString info;
        if (!inFence && isFenceLine(line, &marker, &info)) {
            inFence = true;
            fenceMarker = marker;
            language = info;
            codeLines.clear();
            continue;
        }

        if (inFence) {
            if (line.trimmed().startsWith(fenceMarker)) {
                const QString token = QStringLiteral("REARK_CODE_BLOCK_%1").arg(codeBlocks->size());
                codeBlocks->append(MarkdownCodeBlock {
                    token,
                    normalizedLanguage(language),
                    codeLines.join(QLatin1Char('\n'))
                });
                output += QLatin1Char('\n');
                output += token;
                output += QStringLiteral("\n\n");
                inFence = false;
                fenceMarker.clear();
                language.clear();
                codeLines.clear();
            } else {
                codeLines.append(line);
            }
            continue;
        }

        output += line;
        output += QLatin1Char('\n');
    }

    if (inFence) {
        output += fenceMarker;
        if (!language.isEmpty()) {
            output += QLatin1Char(' ');
            output += language;
        }
        output += QLatin1Char('\n');
        output += codeLines.join(QLatin1Char('\n'));
    }

    static const QRegularExpression imageWithUrl(
        QStringLiteral("!\\[([^\\]]*)\\]\\(([^\\)\\s]+)(?:\\s+\"[^\"]*\")?\\)"));
    output.replace(imageWithUrl, QStringLiteral("[\\1](\\2)"));

    static const QRegularExpression referenceImage(
        QStringLiteral("!\\[([^\\]]*)\\]\\[[^\\]]*\\]"));
    output.replace(referenceImage, QStringLiteral("\\1"));

    return output;
}

QString MarkdownRenderer::renderCodeBlock(const MarkdownCodeBlock& block, bool darkTheme) const
{
    const CodeTheme theme = codeThemeForId(darkTheme ? QStringLiteral("GitHub Dark") : QStringLiteral("GitHub Light"), darkTheme);
    return htmlTableCodeBlock(
        displayLanguage(block.language),
        highlightCode(block.code, block.language, darkTheme),
        block.code,
        darkTheme,
        theme);
}

QString MarkdownRenderer::highlightCode(const QString& code, const QString& language, bool darkTheme) const
{
    const CodeTheme theme = codeThemeForId(darkTheme ? QStringLiteral("GitHub Dark") : QStringLiteral("GitHub Light"), darkTheme);
    if (isPlainTextLanguage(language)) {
        return code.toHtmlEscaped().replace(QLatin1Char('\n'), QStringLiteral("<br/>"));
    }

    QString html;
    html.reserve(code.size() * 2);

    bool inBlockComment = false;
    const QStringList lines = code.split(QLatin1Char('\n'));
    for (int i = 0; i < lines.size(); ++i) {
        html += highlightedLine(QStringView(lines.at(i)), &inBlockComment, theme);
        if (i + 1 < lines.size()) {
            html += QStringLiteral("<br/>");
        }
    }

    return html;
}

QString MarkdownRenderer::styleSheet(bool darkTheme) const
{
    const QString text = darkTheme ? QStringLiteral("#eef5ff") : QStringLiteral("#0f172a");
    const QString muted = darkTheme ? QStringLiteral("#98a7bb") : QStringLiteral("#64748b");
    const QString link = darkTheme ? QStringLiteral("#8fb3ff") : QStringLiteral("#315bdc");
    const QString codeText = darkTheme ? QStringLiteral("#d8e5f5") : QStringLiteral("#172033");
    const QString codeBackground = darkTheme ? QStringLiteral("#0f1722") : QStringLiteral("#eef3f9");
    const QString codeBorder = darkTheme ? QStringLiteral("#2a3748") : QStringLiteral("#d5deeb");
    const QString quoteBorder = darkTheme ? QStringLiteral("#44546a") : QStringLiteral("#c6d2e2");
    const QString quoteBackground = darkTheme ? QStringLiteral("#121b27") : QStringLiteral("#f4f7fb");

    return QStringLiteral(R"(
body {
  color: %1;
  margin: 0;
  font-size: 13px;
  line-height: 1.48;
}
h1, h2, h3, h4, h5, h6 {
  color: %1;
  font-weight: 600;
  margin-top: 13px;
  margin-bottom: 7px;
}
h1 {
  font-size: 20px;
}
h2 {
  font-size: 18px;
}
h3, h4, h5, h6 {
  font-size: 15px;
}
p {
  margin-top: 0;
  margin-bottom: 10px;
}
p:last-child {
  margin-bottom: 0;
}
a {
  color: %3;
  text-decoration: none;
}
strong {
  font-weight: 600;
}
ul, ol {
  margin-top: 6px;
  margin-bottom: 11px;
  margin-left: 0;
  padding-left: 24px;
}
li {
  margin-top: 4px;
  margin-bottom: 4px;
}
pre {
  color: %4;
  background-color: %5;
  border: 1px solid %6;
  border-radius: 6px;
  font-family: Consolas, "Courier New", monospace;
  margin-top: 12px;
  margin-bottom: 14px;
  padding: 11px 12px;
  white-space: pre-wrap;
  line-height: 1.42;
}
code {
  color: %4;
  background-color: %5;
  border-radius: 4px;
  font-family: Consolas, "Courier New", monospace;
  padding-left: 5px;
  padding-right: 5px;
}
blockquote {
  color: %2;
  background-color: %8;
  border-left: 3px solid %7;
  margin: 10px 0;
  padding: 8px 11px;
}
hr {
  color: %6;
  background-color: %6;
  height: 1px;
  border: none;
  margin-top: 14px;
  margin-bottom: 14px;
}
table {
  border-collapse: collapse;
  margin-top: 10px;
  margin-bottom: 12px;
}
th, td {
  border: 1px solid %6;
  padding: 6px 9px;
}
th {
  background-color: %5;
}
)").arg(text, muted, link, codeText, codeBackground, codeBorder, quoteBorder, quoteBackground);
}

void MarkdownRenderer::rememberRenderedHtml(const QString& key, const QString& html) const
{
    if (cache_.contains(key)) {
        return;
    }
    cache_.insert(key, html);
    cacheOrder_.append(key);
}
