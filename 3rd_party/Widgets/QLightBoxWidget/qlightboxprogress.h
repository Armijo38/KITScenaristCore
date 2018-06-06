#ifndef QLIGHTBOXPROGRESS_H
#define QLIGHTBOXPROGRESS_H

#include "qlightboxwidget.h"

class QLabel;
class QProgressBar;


/**
 * @brief Виджет прогресса
 */
class QLightBoxProgress : public QLightBoxWidget
{
    Q_OBJECT

public:
    explicit QLightBoxProgress(QWidget* _parent, bool _folowToHeadWidget = true);
    ~QLightBoxProgress();

    /**
     * @brief Показать виджет с заданными заголовком и описание
     */
    void showProgress(const QString& _title, const QString& _description);

    /**
     * @brief Обновить текст отображаемый в виджете
     */
    static void setProgressText(const QString& _title, const QString& _description);

    /**
     * @brief Установить значение прогресса
     * @note Если значение меньше нуля, или больше ста, то значение прогресса не отображается
     */
    /** @{ */
    static void setProgressValue(int _value);
    static void setProgressValue(int _value, int _maximum);
    /** @} */

    /**
     * @brief Скрыть виджет и просигнализировать пользователю, о том, что операция завершилась
     */
    void finish();

private:
    /**
     * @brief Последний использовавшийся виджет прогресса
     */
    static QLightBoxProgress* s_lastUsedWidget;

    /**
     * @brief Заголовок
     */
    QLabel* m_title = nullptr;

    /**
     * @brief Описание
     */
    QLabel* m_description = nullptr;

    /**
     * @brief Полоса прогресса загрузки
     */
    QProgressBar* m_progress = nullptr;
};

#endif // QLIGHTBOXPROGRESS_H
