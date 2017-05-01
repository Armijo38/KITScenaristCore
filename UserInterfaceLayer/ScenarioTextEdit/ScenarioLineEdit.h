#ifndef SCENARIOLINEEDIT_H
#define SCENARIOLINEEDIT_H

#include "ScenarioTextEdit.h"

namespace UserInterface
{
	/**
	 * @brief Редактор строки сценария
	 */
	class ScenarioLineEdit : public ScenarioTextEdit
	{
		Q_OBJECT

	public:
		explicit ScenarioLineEdit(QWidget* _parent = 0);

	protected:
		/**
		 * @brief Переопределяется для отправки на обработку выше кнопок Enter и Esc
		 */
		void keyPressEvent(QKeyEvent* _event);

		void insertFromMimeData(const QMimeData* _source);

	private slots:
		/**
		 * @brief Убрать переносы строк из текста
		 */
		void removeLineBreaks();

	private:
		/**
		 * @brief Флаг для предовращения рекурсивного вызова функции clearCharFormat
		 */
		bool m_inClearCharFormat;
	};
}

#endif // SCENARIOLINEEDIT_H
