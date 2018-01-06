#include "FountainImporter.h"

#include <BusinessLayer/ScenarioDocument/ScenarioTemplate.h>

#include <QDomDocument>
#include <QFile>
#include <QStack>
#include <QXmlStreamWriter>

using namespace BusinessLogic;

namespace {
/**
     * @brief Ключ для формирования xml из импортируемого документа
     */
/** @{ */
const QString NODE_SCENARIO = "scenario";
const QString NODE_VALUE = "v";
const QString NODE_FORMAT_GROUP = "formatting";
const QString NODE_FORMAT = "format";

const QString ATTRIBUTE_FORMAT_FROM = "from";
const QString ATTRIBUTE_FORMAT_LENGTH = "length";
const QString ATTRIBUTE_FORMAT_BOLD = "bold";
const QString ATTRIBUTE_FORMAT_ITALIC = "italic";
const QString ATTRIBUTE_FORMAT_UNDERLINE = "underline";

const QString ATTRIBUTE_VERSION = "version";
/** @} */

/**
  * @brief С чего может начинаться название сцены
  */
const QStringList sceneHeadings = {QApplication::translate("BusinessLayer::FountainImporter", "INT"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "EXT"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "EST"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "INT./EXT"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "INT/EXT"),
                                   QApplication::translate("BusinessLayer::FountainImporter", "I/E")};

const QMap<QString, QString> TITLE_KEYS({std::make_pair("Title", "name"),
                                        std::make_pair("Author", "author"),
                                        std::make_pair("Authors", "author"),
                                        std::make_pair("Draft date", "year"),
                                        std::make_pair("Contact", "contacts"),
                                        std::make_pair("Credit", "genre"),
                                        std::make_pair("Source", "additional_info")});

const QString TRIPLE_WHITESPACE = "   ";

}

FountainImporter::FountainImporter() :
    AbstractImporter()
{

}

QString FountainImporter::importScenario(const ImportParameters &_importParameters) const
{
    QString scenarioXml;

    //
    // Открываем файл
    //
    QFile fountainFile(_importParameters.filePath);
    if (fountainFile.open(QIODevice::ReadOnly)) {
        //
        // Читаем plain text
        //
        // ... и пишем в сценарий
        //
        QXmlStreamWriter writer(&scenarioXml);
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(true);
        writer.writeStartDocument();
        writer.writeStartElement(NODE_SCENARIO);
        writer.writeAttribute(ATTRIBUTE_VERSION, "1.0");

        //
        // Текст сценария
        //
        QVector<QString> paragraphs;
        bool isTitle = false;
        bool isFirstLine = true;
        for (const QString& str : QString(fountainFile.readAll()).split("\n")) {
            //
            // Если первая строка содержит ':', то в начале идет титульная страница, которую мы обрабатываем не здесь
            //
            if (isFirstLine) {
                isFirstLine = false;
                if (str.contains(':')) {
                    isTitle = true;
                }
            }
            if (isTitle) {
                //
                // Титульная страница заканчивается пустой строкой
                //
                if (str.trimmed().isEmpty()) {
                    isTitle = false;
                }
            } else {
                paragraphs.push_back(str.trimmed());
            }
        }

        const int paragraphsCount = paragraphs.size();
        ScenarioBlockStyle::Type prevBlockType = ScenarioBlockStyle::Undefined;
        QStack<QString> dirs;
        ScenarioBlockStyle::Type blockType;
        for (int i = 0; i != paragraphsCount; ++i) {
            if (notation
                    || commenting) {
                //
                // Если мы комментируем или делаем заметку, то продолжим это
                //
                processBlock(writer, paragraphs[i], prevBlockType);
                continue;
            }

            if (paragraphs[i].isEmpty()) {
                continue;
            }

            blockType = ScenarioBlockStyle::Action;
            QString paragraphText;

            switch(paragraphs[i].toStdString()[0]) {
                case '.':
                {
                    blockType = ScenarioBlockStyle::SceneHeading;
                    //
                    // TODO: номера сцен игнорируем, поскольку в фонтане они являются строками
                    //
                    int sharpPos = paragraphs[i].size();
                    if (paragraphs[i].endsWith("#")) {
                        sharpPos = paragraphs[i].lastIndexOf('#', paragraphs[i].size() - 2);
                    }
                    if (sharpPos == -1) {
                        sharpPos = paragraphs[i].size();
                    }
                    paragraphText = paragraphs[i].mid(1, sharpPos - 1);
                    break;
                }

                case '!':
                {
                    blockType = ScenarioBlockStyle::Action;
                    paragraphText = paragraphs[i].mid(1);
                    break;
                }

                case '@':
                {
                    blockType = ScenarioBlockStyle::Character;
                    paragraphText = paragraphs[i].mid(1);
                    break;
                }

                case '>':
                {
                    if (paragraphs[i].endsWith("<")) {
                        blockType = ScenarioBlockStyle::Action;
                        paragraphText = paragraphs[i].mid(1, paragraphs[i].size() - 2);
                    } else {
                        blockType = ScenarioBlockStyle::Transition;
                        paragraphText = paragraphs[i].mid(1);
                    }
                    break;
                }

                case '=':
                {
                    bool isPageBreak = false;
                    if (paragraphs[i].startsWith("===")) {
                        isPageBreak = true;
                        for (int j = 3; j != paragraphs[i].size(); ++j) {
                            if (paragraphs[i][j] != '=') {
                                isPageBreak = false;
                                break;
                            }
                        }

                        //
                        // Если состоит из трех или более '=', то это PageBreak
                        // У нас такого сейчас нет
                        //
                        continue;
                    }
                    if (!isPageBreak) {
                        blockType = ScenarioBlockStyle::SceneDescription;
                        paragraphText = paragraphs[i].mid(1);
                    }
                    break;
                }

                case '~':
                {
                    //
                    // Лирика
                    //
                    blockType = ScenarioBlockStyle::Lyrics;
                    paragraphText = paragraphs[i].mid(1);
                    break;
                }

                case '#':
                {
                    //
                    // Директории
                    //
                    int sharpCount = 0;
                    while(paragraphs[i].toStdString()[sharpCount] == '#') {
                        ++sharpCount;
                    }

                    if (sharpCount <= dirs.size()) {
                        //
                        // Закроем нужное число раз уже открытые
                        //
                        unsigned toClose = dirs.size() - sharpCount + 1;
                        for (unsigned i = 0; i != toClose; ++i) {
                            processBlock(writer, "КОНЕЦ " + dirs.top(), ScenarioBlockStyle::FolderFooter);
                            dirs.pop();
                        }
                        prevBlockType = ScenarioBlockStyle::FolderFooter;
                    }
                    //
                    // И откроем новую
                    //
                    QString text = paragraphs[i].mid(sharpCount);
                    processBlock(writer, text, ScenarioBlockStyle::FolderHeader);
                    dirs.push(text);
                    prevBlockType = ScenarioBlockStyle::FolderHeader;

                    //
                    // Поскольку директории добавляются прямо здесь без обработки, то в конец цикла идти не надо
                    //
                    continue;
                    break;
                }

                default:
                {
                    bool startsWithHeading = false;
                    for (const QString &sceneHeading : sceneHeadings) {
                        if (paragraphs[i].startsWith(sceneHeading)) {
                            startsWithHeading = true;
                            break;
                        }
                    }

                    if (startsWithHeading
                            && i + 1 < paragraphsCount
                            && paragraphs[i + 1].isEmpty()) {
                        //
                        // Если начинается с одного из времен действия, а после обязательно пустая строка
                        // Значит это заголовок сцены
                        //
                        blockType = ScenarioBlockStyle::SceneHeading;

                        //
                        // TODO: номера сцен игнорируем, поскольку в фонтане они являются строками
                        //
                        int sharpPos = paragraphs[i].size();
                        if (paragraphs[i].startsWith("#")) {
                            sharpPos = paragraphs[i].lastIndexOf('#', paragraphs[i].size() - 2);
                        }
                        if (sharpPos == -1) {
                            sharpPos = paragraphs[i].size();
                        }
                        paragraphText = paragraphs[i].left(sharpPos);
                    } else if (paragraphs[i].startsWith("[[")
                               && paragraphs[i].endsWith("]]")) {
                        //
                        // Редакторская заметка
                        //
                        notes.append(std::make_tuple(paragraphs[i].mid(2, paragraphs[i].size() - 4), noteStartPos, noteLen));
                        noteStartPos += noteLen;
                        noteLen = 0;
                        continue;
                    } else if (paragraphs[i].startsWith("/*")) {
                        //
                        // Начинается комментарий
                        paragraphText = paragraphs[i];
                    } else if (paragraphs[i] == paragraphs[i].toUpper()
                               && i != 0
                               && paragraphs[i-1].isEmpty()
                               && i + 1 < paragraphsCount
                               && paragraphs[i+1].isEmpty()
                               && paragraphs[i].endsWith("TO:")) {
                        //
                        // Если состоит только из заглавных букв, предыдущая и следующая строки пустые
                        // и заканчивается "TO:", то это переход
                        //
                        blockType = ScenarioBlockStyle::Transition;
                        paragraphText = paragraphs[i].left(paragraphs[i].size()-4);
                    } else if (paragraphs[i].startsWith("(")
                               && paragraphs[i].endsWith(")")
                               && (prevBlockType == ScenarioBlockStyle::Character
                                   || prevBlockType == ScenarioBlockStyle::Dialogue)) {
                        //
                        // Если текущий блок обернут в (), то это ремарка
                        //
                        blockType = ScenarioBlockStyle::Parenthetical;
                        paragraphText = paragraphs[i];
                    } else if (paragraphs[i] == paragraphs[i].toUpper()
                               && i != 0
                               && paragraphs[i-1].isEmpty()
                               && i + 1 < paragraphsCount
                               && !paragraphs[i+1].isEmpty()) {
                        //
                        // Если состоит из только из заглавных букв, впереди не пустая строка, а перед пустая
                        // Значит это имя персонажа (для реплики)
                        //
                        blockType = ScenarioBlockStyle::Character;
                        if (paragraphs[i].endsWith("^")) {
                            //
                            // Двойной диалог, который мы пока что не умеем обрабатывать
                            //
                            paragraphText = paragraphs[i].left(paragraphs[i].size() - 1);
                        } else {
                            paragraphText = paragraphs[i];
                        }
                    } else if (prevBlockType == ScenarioBlockStyle::Character
                               || prevBlockType == ScenarioBlockStyle::Parenthetical) {
                        //
                        // Если предыдущий блок - имя персонажа или ремарка, то сейчас диалог
                        //
                        blockType = ScenarioBlockStyle::Dialogue;
                        paragraphText = paragraphs[i];
                    } else {
                        //
                        // Во всех остальных случаях - Action
                        //
                        blockType = ScenarioBlockStyle::Action;
                        paragraphText = paragraphs[i];
                    }
                }
            }
            //
            // Отправим блок на обработку
            //
            processBlock(writer, paragraphText, blockType);
            prevBlockType = blockType;
        }
        //
        // Добавим комментарии к последнему блоку
        //
        appendComments(writer);

        //
        // Закроем последний блок
        //
        writer.writeEndElement();

        //
        // Закроем директории нужное число раз
        //
        while (!dirs.empty()) {
            processBlock(writer, "КОНЕЦ " + dirs.top(), ScenarioBlockStyle::FolderFooter);
            dirs.pop();
        }

        //
        // Закроем документ
        //
        writer.writeEndElement();
        writer.writeEndDocument();
    }

    return scenarioXml;
}

QVariantMap FountainImporter::importResearch(const ImportParameters &_importParameters) const
{
    QString scriptXml;
    //
    // Открываем файл
    //
    QVariantMap scriptResult;
    QFile fountainFile(_importParameters.filePath);
    if (fountainFile.open(QIODevice::ReadOnly)) {
        //
        // Читаем plain text
        //
        QXmlStreamWriter writer(&scriptXml);
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(true);
        writer.writeStartDocument();
        writer.writeStartElement(NODE_SCENARIO);
        writer.writeAttribute(ATTRIBUTE_VERSION, "1.0");

        //
        // Титульная страница сценария
        //
        QStringList text = QString(fountainFile.readAll()).split('\n');
        for (QString& line : text) {
            if (line.endsWith('\r')) {
                line.remove(line.size() - 1, 1);
            }
        }

        //
        // Если есть титульная страница, значит в первой строке ":"
        //
        if (!text.first().contains(":")) {
            return QVariantMap();
        }

        //
        // Титульная страница представлена в виде "key: value"
        //
        bool isMultiLine = false; //Является ли текущий value многострочным
        QString key;
        QString value;
        for (QStringList::const_iterator iter = text.begin(); iter != text.end(); ++iter) {
            //
            // Добрались до пустой строчки. Закончилась титульная страница
            //
            if (iter->trimmed().isEmpty()) {
                break;
            }

            if (!isMultiLine) {
                QStringList splt = iter->split(":");
                if (TITLE_KEYS.contains(splt[0])) {
                    //
                    // Этот параметр нам известен
                    //
                    key = TITLE_KEYS[splt[0]];
                } else {
                    //
                    // Неизвестные параметры пропускаем
                    //
                    key.clear();
                }

                //
                // Значение параметра многострочное
                //
                if (splt.size() == 1
                        || splt[1].isEmpty()) {
                    isMultiLine = true;
                } else {
                    //
                    // Если параметр не многострочен и известен нам
                    // тогда сразу же добавим
                    //
                    if (!key.isEmpty()) {
                        scriptResult[key] = simplify(splt[1].trimmed());
                    }
                }
            } else {
                //
                // Многострочный комментарий.
                // Каждая строка должна начинаться либо с табуляции, либо с 3 пробелов минимум
                //
                if(iter->startsWith(TRIPLE_WHITESPACE)
                        || iter->startsWith('\t')) {

                    //
                    // Добавим перевод новой строки если необходимо и значение
                    //
                    if (!value.isEmpty()) {
                        value += '\n';
                    }
                    value += iter->trimmed();
                } else {
                    //
                    // Закончился многострочный комментарий
                    //
                    if (!key.isEmpty()) {
                        scriptResult[key] = simplify(value);
                    }
                    isMultiLine = false;

                    //
                    // Обработаем текущую строку еще раз, но уже как ключ: значение
                    //
                    --iter;
                    continue;
                }

            }
        }
    }
    QVariantMap result;
    result["script"] = scriptResult;
    return result;
}

void FountainImporter::processBlock(QXmlStreamWriter& writer, QString paragraphText,
                                    ScenarioBlockStyle::Type type) const
{
    if (!notation
            && !commenting) {
        //
        // Начинается новая сущность
        //
        text.reserve(paragraphText.size());

        //
        // Добавим комментарии к предыдущему блоку
        //
        appendComments(writer);

        noteLen = 0;
        noteStartPos = 0;
    }

    char prevSymbol = '\0';
    QVector<TextFormat> formats;
    QVector<TextFormat> tmpFormats;
    int asteriskLen = 0;

    for (int i = 0; i != paragraphText.size(); ++i) {
        if (prevSymbol == '\\') {
            //
            // Если предыдущий символ - \, то просто добавим текущий
            //
            if (notation) {
                note.append(paragraphText[i]);
            }
            else {
                text.append(paragraphText[i]);
            }
            continue;
        }
        char curSymbol = paragraphText.toStdString()[i];
        switch (curSymbol) {
            case '\\':
            {
                if (notation) {
                    note.append(paragraphText[i]);
                } else {
                    text.append(paragraphText[i]);
                }
                break;
            }

            case '/':
            {
                if (prevSymbol == '*'
                        && commenting) {
                    //
                    // Заканчивается комментирование
                    //
                    commenting = false;
                    noteStartPos += noteLen;
                    noteLen = text.size() - 1;

                    //
                    // Закроем предыдущий блок, добавим текущий
                    //
                    writer.writeEndElement();
                    appendBlock(writer, text.left(text.size() - 1), formats, ScenarioBlockStyle::NoprintableText);
                    text.clear();
                } else {
                    if (notation) {
                        note.append('/');
                    } else {
                        text.append('/');
                    }
                }
                break;
            }

            case '*':
            {
                if (prevSymbol == '/'
                        && !commenting
                        && !notation) {
                    //
                    // Начинается комментирование
                    //
                    commenting = true;
                    noteStartPos += noteLen;
                    noteLen = text.size() - 1;

                    //
                    // Закроем предыдущий блок и, если комментирование начинается в середние текущего блока
                    // то добавим этот текущий блок
                    //
                    writer.writeEndElement();
                    if (text.size() != 1) {
                        appendBlock(writer, text.left(text.size() - 1), formats, type);
                        appendComments(writer);
                        notes.clear();
                    }
                    text.clear();
                } else {
                    if (notation) {
                        note.append('*');
                    } else {
                        //
                        // Игнорируем *, поскольку они являются символом форматирования, которое мы еще не умеем
                        //
                        ++asteriskLen;
                        //text.append('*');
                    }
                }
                break;
            }

            case '[':
            {
                if (prevSymbol == '['
                        && !commenting
                        && !notation) {
                    //
                    // Начинается редакторская заметка
                    //
                    notation = true;
                    noteLen = text.size() - 1 - noteStartPos;
                    text = text.left(text.size() - 1);
                } else {
                    if (notation) {
                        note.append('[');
                    } else {
                        text.append('[');
                    }
                }
                break;
            }

            case ']':
            {
                if (prevSymbol == ']'
                        && notation) {
                    //
                    // Закончилась редакторская заметка. Добавим ее в список редакторских заметок к текущему блоку
                    //
                    notation = false;
                    notes.append(std::make_tuple(note.left(note.size() - 1), noteStartPos, noteLen));
                    noteStartPos += noteLen;
                    noteLen = 0;
                    note.clear();
                } else {
                    if (notation) {
                        note.append(']');
                    } else {
                        text.append(']');
                    }
                }
                break;
            }

            case '_':
                //
                // Подчеркивания обрабатываются в другом месте
                //
                break;
            default:
            {
                //
                // Самый обычный символ
                //
                if (notation) {
                    note.append(paragraphText[i]);
                } else {
                    text.append(paragraphText[i]);
                }
                break;
            }
        }

        //
        // Underline
        //
        if (prevSymbol == '_') {
            addFormat(formats, tmpFormats, false, false, true, curSymbol == '*');
        }

        prevSymbol = paragraphText.toStdString()[i];

        if (curSymbol != '*') {
            switch(asteriskLen) {
                //
                // Italics
                //
            case 1:
                addFormat(formats, tmpFormats, true, false, false, curSymbol == '_');
                break;
                //
                // Bold
                //
            case 2:
                addFormat(formats, tmpFormats, false, true, false, curSymbol == '_');
                break;
                //
                // Bold & Italics
                //
            case 3:
                addFormat(formats, tmpFormats, true, true, false, curSymbol == '_');
                break;
            default:
                break;
            }
            asteriskLen = 0;
        }
    }

    //
    // Underline
    //
    if (prevSymbol == '_') {
        addFormat(formats, tmpFormats, false, false, true, true);
    }

    switch(asteriskLen) {
        //
        // Italics
        //
    case 1:
        addFormat(formats, tmpFormats, true, false, false);
        break;
        //
        // Bold
        //
    case 2:
        addFormat(formats, tmpFormats, false, true, false);
        break;
        //
        // Bold & Italics
        //
    case 3:
        addFormat(formats, tmpFormats, true, true, false);
        break;
    default:
        break;
    }
    asteriskLen = 0;


    if (!notation
            && !commenting) {
        //
        // Если блок действительно закончился
        //
        noteLen += text.size() - noteStartPos;

        //
        // Закроем предыдущий блок
        //
        if (!firstBlock) {
            writer.writeEndElement();
        }

        //
        // Добавим текущий блок
        //
        if (!text.isEmpty() || type == ScenarioBlockStyle::FolderFooter) {
            appendBlock(writer, text, formats, type);
        }
        text.clear();
    }

    //
    // Первый блок в тексте может встретиться лишь однажды
    //
    if (!firstBlock) {
        firstBlock = false;
    }
}

void FountainImporter::appendBlock(QXmlStreamWriter &writer, const QString &paragraphText,
                                   QVector<TextFormat> &_formats, ScenarioBlockStyle::Type type) const
{
    const QString& blockTypeName = ScenarioBlockStyle::typeName(type);
    writer.writeStartElement(blockTypeName);
    writer.writeStartElement(NODE_VALUE);
    writer.writeCDATA(paragraphText.trimmed());
    writer.writeEndElement();

    if (!_formats.isEmpty()) {
        writer.writeStartElement(NODE_FORMAT_GROUP);
        for (const TextFormat& format: _formats) {
            writer.writeStartElement(NODE_FORMAT);
            //
            // Данные пользовательского форматирования
            //
            writer.writeAttribute(ATTRIBUTE_FORMAT_FROM, QString::number(format.start));
            writer.writeAttribute(ATTRIBUTE_FORMAT_LENGTH, QString::number(format.length));
            writer.writeAttribute(ATTRIBUTE_FORMAT_BOLD, format.bold ? "true" : "false");
            writer.writeAttribute(ATTRIBUTE_FORMAT_ITALIC, format.italic? "true" : "false");
            writer.writeAttribute(ATTRIBUTE_FORMAT_UNDERLINE, format.underline ? "true" : "false");
            //
            writer.writeEndElement();
        }
        writer.writeEndElement();
        _formats.clear();
    }

    //
    // Не закрываем блок, чтобы можно было добавить редакторских заметок
    //
    //writer.writeEndElement();
}

void FountainImporter::appendComments(QXmlStreamWriter &writer) const
{
    if (notes.isEmpty()) {
        return;
    }

    writer.writeStartElement("reviews");

    for (int i = 0; i != notes.size(); ++i) {
        if (std::get<2>(notes[i]) != 0) {
            if (i != 0) {
                writer.writeEndElement();
            }
            writer.writeStartElement("review");
            writer.writeAttribute("from", QString::number(std::get<1>(notes[i])));
            writer.writeAttribute("length", QString::number(std::get<2>(notes[i])));
            writer.writeAttribute("bgcolor", "#ffff00");
            writer.writeAttribute("is_highlight", "true");
        }
        writer.writeEmptyElement("review_comment");
        writer.writeAttribute("comment", std::get<0>(notes[i]));
    }

    writer.writeEndElement();
    writer.writeEndElement();

    notes.clear();
}

QString FountainImporter::simplify(const QString &_value) const
{
    QString res;
    for (int i = 0; i != _value.size(); ++i) {
        if (_value[i] == '*'
                || _value[i] == '_'
                || _value[i] == '\\') {
            if (i == 0
                      ||(  i > 0
                    && _value[i-1] != '\\')) {
                continue;
            } else {
               res += _value[i];
            }
        }
        else {
            res += _value[i];
        }
    }
    return res;
}

void FountainImporter::addFormat(QVector<AbstractImporter::TextFormat> &formats,
                                 QVector<AbstractImporter::TextFormat> &tmpFormats,
                                 bool isItalics, bool isBold, bool isUnderline,
                                 bool byOne) const
{
    if (tmpFormats.isEmpty()) {
        //
        // Новый формат, который еще не начат
        //
            TextFormat format;
            format.bold = isBold;
            format.italic = isItalics;
            format.underline = isUnderline;
            format.start = text.size();
            if (!byOne) {
                --format.start;
            }
            tmpFormats.push_back(format);
    } else {
        //
        // Формат уже начат
        //
        TextFormat& format = tmpFormats[0];
        //
        // Либо закроем его полностью
        //
        if (format.bold == isBold
                && format.italic == isItalics
                && format.underline == isUnderline) {
            format.length = text.size() - format.start;
            if (!byOne) {
                --format.length;
            }
            if (format.length != 0) {
                formats.push_back(format);
            }
            tmpFormats.clear();
        } else {
            //
            // Либо закроем и создадим новый, частично унаследованный от предыдущего
            //
            TextFormat newFormat;
            newFormat.italic = format.italic ^ isItalics;
            newFormat.bold = format.bold ^ isBold;
            newFormat.underline = format.underline ^ isUnderline;
            format.length = text.size() - format.start;
            if (!byOne) {
                --format.length;
            }
            if (format.length != 0) {
                formats.append(format);
            }
            newFormat.start = format.start + format.length;
            tmpFormats.clear();
            tmpFormats.append(newFormat);
        }
    }
}

