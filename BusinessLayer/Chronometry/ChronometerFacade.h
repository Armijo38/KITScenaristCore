#ifndef CHRONOMETERFACADE_H
#define CHRONOMETERFACADE_H

#include <QString>

class QTextBlock;
class QTextDocument;

namespace BusinessLogic
{
	class AbstractChronometer;


	/**
	 * @brief Фасад для доступа к рассчёту хронометража
	 */
	class ChronometerFacade
	{
	public:
		/**
		 * @brief Используется ли хронометраж
		 */
		static bool chronometryUsed();

		/**
		 * @brief Вычислить хронометраж последовательности ограниченной заданным блоком
		 */
		static qreal calculate(const QTextBlock& _block);

		/**
		 * @brief Вычислить хронометраж последовательности ограниченной заданными блоками
		 */
		static qreal calculate(const QTextBlock& _fromBlock, const QTextBlock& _toBlock);

		/**
		 * @brief Вычислить хронометраж последовательности ограниченной заданными позициями
		 */
		static qreal calculate(QTextDocument* _document, int _fromCursorPosition, int _toCursorPosition);

		/**
		 * @brief Вычислить хронометраж всего документа
		 */
		static qreal calculate(QTextDocument* _document);

		/**
		 * @brief Получить строковое представление для заданного количества секунд
		 */
		/** @{ */
		static QString secondsToTime(int _seconds);
		static QString secondsToTime(qreal _seconds);
		/** @} */

	private:
		/**
		 * @brief Получить необходимый для использования хронометр
		 */
		static AbstractChronometer* chronometer();

	private:
		/**
		 * @brief Текущий хронометр
		 */
		static AbstractChronometer* s_chronometer;
	};
}

#endif // CHRONOMETERFACADE_H
