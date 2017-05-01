#include "TabBar.h"

TabBar::TabBar(QWidget* _parent) :
    QTabBar(_parent)
{
}

QSize TabBar::tabSizeHint(int _index) const
{
    QSize defaultTabSizeHint = QTabBar::tabSizeHint(_index);
    QSize newTabSizeHint(defaultTabSizeHint.width(), height());
    return newTabSizeHint;
}
