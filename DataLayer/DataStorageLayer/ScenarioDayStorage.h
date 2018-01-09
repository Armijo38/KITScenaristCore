#ifndef SCENARIODAYSTORAGE_H
#define SCENARIODAYSTORAGE_H

#include "StorageFacade.h"

class QString;

namespace Domain {
	class ScenarioDay;
	class ScenarioDaysTable;
}

using namespace Domain;


namespace DataStorageLayer
{
	class ScenarioDayStorage
	{
	public:
		/**
		 * @brief Все дни сценария
		 */
		ScenarioDaysTable* all();

		/**
		 * @brief Сохранить сценарный день
		 */
		ScenarioDay* storeScenarioDay(const QString& _scenarioDayName);

        /**
         * @brief Удалить сценарный день
         */
        void removeScenarioDay(const QString& _name);

		/**
		 * @brief Очистить хранилище
		 */
		void clear();

		/**
		 * @brief Обновить хранилище
		 */
		void refresh();

	private:
		ScenarioDaysTable* m_all;

	private:
		ScenarioDayStorage();

		// Для доступа к конструктору
		friend class StorageFacade;
	};
}

#endif // SCENARIODAYSTORAGE_H
