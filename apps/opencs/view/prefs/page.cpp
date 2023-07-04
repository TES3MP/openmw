#include "page.hpp"

#include <utility>
#include <vector>

#include <apps/opencs/view/prefs/pagebase.hpp>

#include <QGridLayout>

#include "../../model/prefs/category.hpp"
#include "../../model/prefs/setting.hpp"

CSVPrefs::Page::Page(CSMPrefs::Category& category, QWidget* parent)
    : PageBase(category, parent)
{
    QWidget* widget = new QWidget(parent);
    mGrid = new QGridLayout(widget);

    for (CSMPrefs::Category::Iterator iter = category.begin(); iter != category.end(); ++iter)
        addSetting(*iter);

    setWidget(widget);
}

void CSVPrefs::Page::addSetting(CSMPrefs::Setting* setting)
{
    std::pair<QWidget*, QWidget*> widgets = setting->makeWidgets(this);

    int next = mGrid->rowCount();

    if (widgets.first)
    {
        mGrid->addWidget(widgets.first, next, 0);
        mGrid->addWidget(widgets.second, next, 1);
    }
    else if (widgets.second)
    {
        mGrid->addWidget(widgets.second, next, 0, 1, 2);
    }
    else
    {
        mGrid->addWidget(new QWidget(this), next, 0);
    }
}
