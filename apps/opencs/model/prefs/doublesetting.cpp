
#include "doublesetting.hpp"

#include <limits>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QMutexLocker>

#include <components/settings/settings.hpp>

#include <apps/opencs/model/prefs/setting.hpp>

#include "category.hpp"
#include "state.hpp"

CSMPrefs::DoubleSetting::DoubleSetting(
    Category* parent, QMutex* mutex, const std::string& key, const std::string& label, double default_)
    : Setting(parent, mutex, key, label)
    , mPrecision(2)
    , mMin(0)
    , mMax(std::numeric_limits<double>::max())
    , mDefault(default_)
    , mWidget(nullptr)
{
}

CSMPrefs::DoubleSetting& CSMPrefs::DoubleSetting::setPrecision(int precision)
{
    mPrecision = precision;
    return *this;
}

CSMPrefs::DoubleSetting& CSMPrefs::DoubleSetting::setRange(double min, double max)
{
    mMin = min;
    mMax = max;
    return *this;
}

CSMPrefs::DoubleSetting& CSMPrefs::DoubleSetting::setMin(double min)
{
    mMin = min;
    return *this;
}

CSMPrefs::DoubleSetting& CSMPrefs::DoubleSetting::setMax(double max)
{
    mMax = max;
    return *this;
}

CSMPrefs::DoubleSetting& CSMPrefs::DoubleSetting::setTooltip(const std::string& tooltip)
{
    mTooltip = tooltip;
    return *this;
}

std::pair<QWidget*, QWidget*> CSMPrefs::DoubleSetting::makeWidgets(QWidget* parent)
{
    QLabel* label = new QLabel(QString::fromUtf8(getLabel().c_str()), parent);

    mWidget = new QDoubleSpinBox(parent);
    mWidget->setDecimals(mPrecision);
    mWidget->setRange(mMin, mMax);
    mWidget->setValue(mDefault);

    if (!mTooltip.empty())
    {
        QString tooltip = QString::fromUtf8(mTooltip.c_str());
        label->setToolTip(tooltip);
        mWidget->setToolTip(tooltip);
    }

    connect(mWidget, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &DoubleSetting::valueChanged);

    return std::make_pair(label, mWidget);
}

void CSMPrefs::DoubleSetting::updateWidget()
{
    if (mWidget)
    {
        mWidget->setValue(Settings::Manager::getFloat(getKey(), getParent()->getKey()));
    }
}

void CSMPrefs::DoubleSetting::valueChanged(double value)
{
    {
        QMutexLocker lock(getMutex());
        Settings::Manager::setFloat(getKey(), getParent()->getKey(), value);
    }

    getParent()->getState()->update(*this);
}
