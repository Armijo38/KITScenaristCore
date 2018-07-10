#include "CharacterState.h"

using namespace Domain;


CharacterState::CharacterState(const Identifier& _id, const QString& _name) :
	DomainObject(_id),
	m_name(_name)
{
}

QString CharacterState::name() const
{
	return m_name;
}

void CharacterState::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;

		changesNotStored();
    }
}

bool CharacterState::equal(const QString& _name) const
{
    return m_name == _name;
}

// ****

namespace {
	const int kColumnCount = 1;
}

CharacterStatesTable::CharacterStatesTable(QObject* _parent) :
	DomainObjectsItemModel(_parent)
{
}

int CharacterStatesTable::columnCount(const QModelIndex&) const
{
	return kColumnCount;
}

QVariant CharacterStatesTable::data(const QModelIndex& _index, int _role) const
{
	QVariant resultData;

	if (_role ==  Qt::DisplayRole
		|| _role == Qt::EditRole) {
		DomainObject *domainObject = domainObjects().value(_index.row());
		CharacterState* characterState = dynamic_cast<CharacterState*>(domainObject);
		Column column = sectionToColumn(_index.column());
		switch (column) {
			case Name: {
				resultData = characterState->name();
				break;
			}

			default: {
				break;
			}
		}
	}

	return resultData;
}

CharacterStatesTable::Column CharacterStatesTable::sectionToColumn(int _section) const
{
	Column column = Undefined;

	switch (_section) {
		case 0: {
			column = Name;
			break;
		}
		default: {
			break;
		}
	}

	return column;
}
