#ifndef __KCMTOUCHPAD_H__
#define __KCMTOUCHPAD_H__

#include <QSet>
#include <QString>

#include <KApplication>
#include <KCModule>

#include "touchpad.h"

class Ui_TouchpadConfigWidget;

class TouchpadConfig : public KCModule
{
  Q_OBJECT

public:
    TouchpadConfig(QWidget *parent, const QVariantList &args);
    ~TouchpadConfig();

    void save();
    void load();
    void defaults();

    QString quickHelp() const;

    static void init_touchpad();

private:
    bool apply();
    void enableProperties();

    Ui_TouchpadConfigWidget* ui;

    /* map events to button: (event) -> (button) */
    QMap<int, int> tappingButtonsMap;

    QSet<const char*> propertiesList;

    int valid;
    
private slots:
    void changed();

    void touchpadEnabled(bool toggle);
    void touchpadAllowedMoving(bool toggle);

    void smartModeEnabled(bool toggle);
    void smartModeDelayChanged(int value);

    void sensitivityValueChanged(int value);

    void scrollVerticalEnabled(bool toggle);
    void scrollVerticalSpeedChanged(int value);
    void scrollVerticalTFEnabled(bool toggle);

    void scrollHorizontalEnabled(bool toggle);
    void scrollHorizontalSpeedChanged(int value);
    void scrollHorizontalTFEnabled(bool toggle);

    void scrollCoastingEnabled(bool toggle);
    void scrollCoastingSpeedChanged(int value);

    void circularScrollEnabled(bool toggle);
    void circularScrollSpeedChanged(int value);
    void circularScrollCornersChosen(int chosen);

    void tappingEnabled(bool toggle);
    void tappingTimeoutChanged(int value);
    void tappingDoubleTimeChanged(int value);
    void tappingClickTimeChanged(int value);

    void tappingEventListSelected(int current);
    void tappingButtonListSelected(int current);
};

#endif

