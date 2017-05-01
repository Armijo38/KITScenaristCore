#ifndef DIALOGHANDLER_H
#define DIALOGHANDLER_H

#include "StandardKeyHandler.h"


namespace KeyProcessingLayer
{
	/**
	 * @brief Класс выполняющий обработку нажатия клавиш в блоке реплики
	 */
	class DialogHandler : public StandardKeyHandler
	{
	public:
		DialogHandler(UserInterface::ScenarioTextEdit* _editor);

	protected:
		/**
		 * @brief Реализация интерфейса AbstractKeyHandler
		 */
		/** @{ */
		void handleEnter(QKeyEvent* _event = 0);
		void handleTab(QKeyEvent* _event = 0);
		void handleOther(QKeyEvent *_event);
		/** @} */
	};
}

#endif // DIALOGHANDLER_H
