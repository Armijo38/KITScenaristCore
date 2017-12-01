#ifndef SCRIPTTEXTCORRECTOR_H
#define SCRIPTTEXTCORRECTOR_H

#include <QObject>
#include <QHash>
#include <QVector>

class QTextBlock;
class QTextCursor;
class QTextDocument;


namespace BusinessLogic
{
    /**
     * @brief Класс корректирующий текст сценария
     */
    class ScriptTextCorrector : public QObject
    {
        Q_OBJECT

    public:
        explicit ScriptTextCorrector(QTextDocument* _document);

        /**
         * @brief Скорректировать текст сценария
         */
        void correct();

    private:
        /**
         * @brief Сместить блок в начало следующей страницы
         * @param _cursor - курсор редактироуемого документа
         * @param _block - блок для смещения
         * @param _spaceToPageEnd - количество места до конца страницы
         * @param _pageHeight - высота страницы
         */
        void moveBlockToNextPage(QTextCursor& _cursor, const QTextBlock& _block, qreal _spaceToPageEnd, qreal _pageHeight);

    private:
        /**
         * @brief Документ который будем корректировать
         */
        QTextDocument* m_document = nullptr;

        /**
         * @brief Структура элемента модели блоков
         */
        struct BlockInfo {
            BlockInfo() = default;
            BlockInfo(qreal _height, qreal _top) :
                height(_height),
                top(_top)
            {}

            /**
             * @brief Высота блока
             */
            qreal height = 0.0;

            /**
             * @brief Позиция блока от начала страницы
             */
            qreal top = 0.0;
        };

        /**
         * @brief Номер текущего блока при корректировке
         * @note Используем собственный счётчик номеров, т.к. во время
         *       коррекций номера блоков могут скакать в QTextBlock
         */
        int m_currentBlockNumber = 0;

        /**
         * @brief Модель блоков <порядковый номер блока, параметры блока>
         */
        QHash<int, BlockInfo> m_blockItems;
    };
}

#endif // SCRIPTTEXTCORRECTOR_H
