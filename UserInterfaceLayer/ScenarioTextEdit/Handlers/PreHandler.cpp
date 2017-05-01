#include "PreHandler.h"

#include "../ScenarioTextEdit.h"

#include <QKeyEvent>
#include <QTextBlock>

using namespace KeyProcessingLayer;
using namespace BusinessLogic;
using UserInterface::ScenarioTextEdit;


PreHandler::PreHandler(ScenarioTextEdit* _editor) :
	StandardKeyHandler(_editor)
{
}

void PreHandler::handleDelete(QKeyEvent* _event)
{
	if (_event == 0) {
		StandardKeyHandler::handleDelete();
	}
}

void PreHandler::handleOther(QKeyEvent* _event)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();

	//
	// Получим стиль первого блока в выделении
	//
	QTextCursor topCursor(editor()->document());
	topCursor.setPosition(qMin(cursor.selectionStart(), cursor.selectionEnd()));
	ScenarioBlockStyle topStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::forBlock(topCursor.block()));

	//
	// Получим стиль последнего блока в выделении
	//
	QTextCursor bottomCursor(editor()->document());
	bottomCursor.setPosition(qMax(cursor.selectionStart(), cursor.selectionEnd()));
	ScenarioBlockStyle bottomStyle = ScenarioTemplateFacade::getTemplate().blockStyle(ScenarioBlockStyle::forBlock(bottomCursor.block()));

	//
	// Не все стили можно редактировать
	//
	if (topStyle.isCanModify()
		&& bottomStyle.isCanModify()) {
		//
		// Если имеется выделение, то удалим все выделенные символы
		//
		if (cursor.hasSelection()
			&& !_event->text().isEmpty()) {
			handleDelete();
		}
	}
}
