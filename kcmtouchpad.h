
#ifndef __KCMTOUCHPAD_H__
#define __KCMTOUCHPAD_H__

#include <KApplication>
#include <KCModule>

class Ui_TouchpadConfigWidget;

class TouchpadConfig : public KCModule
{
  Q_OBJECT

public:
    TouchpadConfig(QWidget *parent, const QVariantList &args);

    /* poniższe funkcje są wirtualnie udostępniane przez KDE
     * należy je zdefiniować w module */
    void save();
    void load();
    void defaults();

    QString quickHelp() const;

    static void init_touchpad();

private:
    /* funkcja do zmiany parametrów touchpada */
    bool apply();

    Ui_TouchpadConfigWidget* ui;

    /* mapa działań na eventy przycisków */
    QMap<int, int> tappingButtonsMap;

    /* flaga poprawności zainicjowania pamięci dzielonej */
    bool valid;
    
private slots:
    /* sloty odbierające sygnały od kontrolek użytkownika */
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
    void tappingTimeChanged(int value);
    void tappingDoubleTimeChanged(int value);

    void tappingEventListSelected(int current);
    void tappingButtonListSelected(int current);
};

#endif

