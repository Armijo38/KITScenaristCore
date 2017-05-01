#include "StorageFacade.h"

#include "PlaceStorage.h"
#include "ScenarioDayStorage.h"
#include "TimeStorage.h"
#include "CharacterStateStorage.h"
#include "ScenarioStorage.h"
#include "ScenarioChangeStorage.h"
#include "ScenarioDataStorage.h"
#include "ResearchStorage.h"
#include "SettingsStorage.h"
#include "DatabaseHistoryStorage.h"

using namespace DataStorageLayer;


QString StorageFacade::username()
{
    //
    // Если пользователь авторизован, то используем его логин
    //
    const QString login = settingsStorage()->value("application/email", SettingsStorage::ApplicationSettings);
    if (!login.isEmpty()) {
        return login;
    }

    //
    // А если не авторизован, то используем имя пользователя из системы
    //
    return settingsStorage()->value("application/user-name", SettingsStorage::ApplicationSettings);
}

void StorageFacade::clearStorages()
{
    placeStorage()->clear();
    scenarioDayStorage()->clear();
    timeStorage()->clear();
    characterStateStorage()->clear();
    scenarioStorage()->clear();
    scenarioChangeStorage()->clear();
    scenarioDataStorage()->clear();
    researchStorage()->clear();
}

void StorageFacade::refreshStorages()
{
    researchStorage()->refresh();
    placeStorage()->refresh();
    scenarioDayStorage()->refresh();
    timeStorage()->refresh();
    characterStateStorage()->refresh();
    scenarioDataStorage()->refresh();

    //
    // Хранилища со списком изменений сценария и сам сценарий обновляются по другому принципу
    //
    //	scenarioStorage()->refresh();
    //	scenarioChangeStorage()->refresh();
}

PlaceStorage* StorageFacade::placeStorage()
{
    if (s_placeStorage == 0) {
        s_placeStorage = new PlaceStorage;
    }
    return s_placeStorage;
}

ScenarioDayStorage* StorageFacade::scenarioDayStorage()
{
    if (s_scenarioDayStorage == 0) {
        s_scenarioDayStorage = new ScenarioDayStorage;
    }
    return s_scenarioDayStorage;
}

TimeStorage* StorageFacade::timeStorage()
{
    if (s_timeStorage == 0) {
        s_timeStorage = new TimeStorage;
    }
    return s_timeStorage;
}

CharacterStateStorage*StorageFacade::characterStateStorage()
{
    if (s_characterStateStorage == 0) {
        s_characterStateStorage = new CharacterStateStorage;
    }
    return s_characterStateStorage;
}

ScenarioStorage* StorageFacade::scenarioStorage()
{
    if (s_scenarioStorage == 0) {
        s_scenarioStorage = new ScenarioStorage;
    }
    return s_scenarioStorage;
}

ScenarioChangeStorage* StorageFacade::scenarioChangeStorage()
{
    if (s_scenarioChangeStorage == 0) {
        s_scenarioChangeStorage = new ScenarioChangeStorage;
    }
    return s_scenarioChangeStorage;
}

ScenarioDataStorage* StorageFacade::scenarioDataStorage()
{
    if (s_scenarioDataStorage == 0) {
        s_scenarioDataStorage = new ScenarioDataStorage;
    }
    return s_scenarioDataStorage;
}

ResearchStorage* StorageFacade::researchStorage()
{
    if (s_researchStorage == 0) {
        s_researchStorage = new ResearchStorage;
    }
    return s_researchStorage;
}

SettingsStorage* StorageFacade::settingsStorage()
{
    if (s_settingsStorage == 0) {
        s_settingsStorage = new SettingsStorage;
    }
    return s_settingsStorage;
}

DatabaseHistoryStorage* StorageFacade::databaseHistoryStorage()
{
    if (s_databaseHistoryStorage == 0) {
        s_databaseHistoryStorage = new DatabaseHistoryStorage;
    }
    return s_databaseHistoryStorage;
}

PlaceStorage* StorageFacade::s_placeStorage = 0;
ScenarioDayStorage* StorageFacade::s_scenarioDayStorage = 0;
TimeStorage* StorageFacade::s_timeStorage = 0;
CharacterStateStorage* StorageFacade::s_characterStateStorage = 0;
ScenarioStorage* StorageFacade::s_scenarioStorage = 0;
ScenarioChangeStorage* StorageFacade::s_scenarioChangeStorage = 0;
ScenarioDataStorage* StorageFacade::s_scenarioDataStorage = 0;
ResearchStorage* StorageFacade::s_researchStorage = 0;
SettingsStorage* StorageFacade::s_settingsStorage = 0;
DatabaseHistoryStorage* StorageFacade::s_databaseHistoryStorage = 0;
