#include <QCheckBox>
#include <QSlider>
#include <QGroupBox>
#include <QLabel>

#include <KButtonGroup>
#include <KApplication>
#include <KConfig>
#include <KGlobal>
#include <KStandardDirs>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KMessageBox>

#include "kcmtouchpad.h"
#include "touchpad.h"
#include "ui_kcmtouchpadwidget.h"

K_PLUGIN_FACTORY(TouchpadConfigFactory,
        registerPlugin<TouchpadConfig>("touchpad");
        )
K_EXPORT_PLUGIN(TouchpadConfigFactory("kcmtouchpad"))

TouchpadConfig::TouchpadConfig(QWidget *parent, const QVariantList &)
        : KCModule(TouchpadConfigFactory::componentData(), parent)
{
    // get instance of singleton
    if(!device->hasShm())
    {
        KMessageBox::error(this, "Shared memory for synaptics driver is not enabled!", "SHM needed");
        // driver's SHM need to be enabled
        valid = false;
        return;
    }
    
    valid = true;

    // set user interface
    ui = new Ui_TouchpadConfigWidget();
    ui->setupUi(this);

    // set library version
    QString text = ui->LibraryVersionL->text();
    text.append(device->libraryStr().c_str());
    ui->LibraryVersionL->setText(text);

    // set driver version
    text = ui->DriverVersionL->text();
    text.append(device->driverStr().c_str());
    ui->DriverVersionL->setText(text);

    /* łączenie wszystkich sygnałów od kontrolek do slotów modułu */
    connect(ui->TouchpadOnRB, SIGNAL(toggled(bool)), this, SLOT(touchpadEnabled(bool)));
    connect(ui->TouchpadOffWOMoveCB, SIGNAL(toggled(bool)), this, SLOT(touchpadAllowedMoving(bool)));

    //connect(ui->SmartModeEnableCB, SIGNAL(toggled(bool)), this, SLOT(smartModeEnabled(bool)));
    //connect(ui->SmartModeDelayS, SIGNAL(valueChanged(int)), this, SLOT(smartModeDelayChanged(int)));

    connect(ui->SensitivityValueS, SIGNAL(valueChanged(int)), this, SLOT(sensitivityValueChanged(int)));

    connect(ui->ScrollVertEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollVerticalEnabled(bool)));
    connect(ui->ScrollVertSpeedS, SIGNAL(valueChanged(int)), this, SLOT(scrollVerticalSpeedChanged(int)));
    connect(ui->ScrollVertTFEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollVerticalTFEnabled(bool)));

    connect(ui->ScrollHorizEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollHorizontalEnabled(bool)));
    connect(ui->ScrollHorizSpeedS, SIGNAL(valueChanged(int)), this, SLOT(scrollHorizontalSpeedChanged(int)));
    connect(ui->ScrollHorizTFEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollHorizontalTFEnabled(bool)));

    connect(ui->ScrollCoastingEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollCoastingEnabled(bool)));
    connect(ui->ScrollCoastingSpeedS, SIGNAL(valueChanged(int)), this, SLOT(scrollCoastingSpeedChanged(int)));

    connect(ui->ScrollCircularEnableCB, SIGNAL(toggled(bool)), this, SLOT(circularScrollEnabled(bool)));
    connect(ui->ScrollCircularSpeedS, SIGNAL(valueChanged(int)), this, SLOT(circularScrollSpeedChanged(int)));
    connect(ui->ScrollCircularCornersCBB, SIGNAL(currentIndexChanged(int)), this, SLOT(circularScrollCornersChosen(int)));

    connect(ui->TappingEnableCB, SIGNAL(toggled(bool)), this, SLOT(tappingEnabled(bool)));
    connect(ui->TappingTimeValueS, SIGNAL(valueChanged(int)), this, SLOT(tappingTimeChanged(int)));
    connect(ui->TappingDoubleTimeValueS, SIGNAL(valueChanged(int)), this, SLOT(tappingDoubleTimeChanged(int)));

    connect(ui->TappingEventLW, SIGNAL(currentRowChanged(int)), this, SLOT(tappingEventListSelected(int)));
    connect(ui->TappingButtonLW, SIGNAL(currentRowChanged(int)), this, SLOT(tappingButtonListSelected(int)));
}

/*
 * Funkcja uruchamiana przez KDE podczas uruchomienia modułu
 * (tak przez systemsettings, kcontrol4 jak i kcmshell4 ).
 * Wczytuje konfigurację touchpada z pliku
 */
void TouchpadConfig::load()
{
    if(!valid)
        return;

    // obiekt konfiguracyjny KDE
    KConfigGroup config(KSharedConfig::openConfig("kcmtouchpadrc", KConfig::NoGlobals), "Touchpad");

    // wszystkie poniższe funkcje ustawiają kontrolki zgodnie z wczytanymi wartościami z konfiguracji
    // jeśli konfiguracji nie ma, zostaje pobrana wartość ze sterownika
    ui->TouchpadOnRB->setChecked(config.readEntry("TouchpadOn", !Touchpad::isTouchpadEnabled()));
    ui->TouchpadOffWOMoveCB->setCheckState(config.readEntry("TouchpadAllowMoving", Touchpad::isTouchpadEnabled() == 2) ? Qt::Checked : Qt::Unchecked);

    //ui->SmartModeEnableCB->setCheckState(config.readEntry("SmartModeEnabled", false) ? Qt::Checked : Qt::Unchecked);
    //ui->SmartModeDelayS->setValue(config.readEntry("SmartModeDelay", 1000));

    ui->SensitivityValueS->setValue(config.readEntry("Sensitivity", Touchpad::sensitivity()));

    ui->ScrollVertEnableCB->setCheckState(config.readEntry("ScrollingVerticalEnabled", Touchpad::isVerticalScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);
    ui->ScrollVertSpeedS->setValue(config.readEntry("ScrollingVerticalSpeed", Touchpad::verticalScrollingDelta()));
    ui->ScrollVertTFEnableCB->setCheckState(config.readEntry("ScrollingVerticalTwoFingersEnabled", Touchpad::isVerticalTwoFingersScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);

    ui->ScrollHorizEnableCB->setCheckState(config.readEntry("ScrollingHorizontalEnabled", Touchpad::isHorizontalScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);
    ui->ScrollHorizSpeedS->setValue(config.readEntry("ScrollingHorizontalSpeed", Touchpad::horizontalScrollingDelta()));
    ui->ScrollHorizTFEnableCB->setCheckState(config.readEntry("ScrollingHorizontalTwoFingersEnabled", Touchpad::isHorizontalTwoFingersScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);

    ui->ScrollCoastingEnableCB->setCheckState(config.readEntry("ScrollingCoastingEnabled", Touchpad::isCoastingEnabled()) ? Qt::Checked : Qt::Unchecked);
    ui->ScrollCoastingSpeedS->setValue(config.readEntry("ScrollingCoastingSpeed", Touchpad::coastingSpeed()));

    ui->ScrollCircularEnableCB->setCheckState(config.readEntry("CircularScrollingEnabled", Touchpad::isCircularScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);
    ui->ScrollCircularSpeedS->setValue(config.readEntry("CircularScrollingSpeed", Touchpad::circularScrollingDelta()));
    ui->ScrollCircularCornersCBB->setCurrentIndex(config.readEntry("CircularScrollingCorners", (int)Touchpad::circularScrollingTrigger()));

    ui->TappingEnableCB->setCheckState(config.readEntry("TappingEnabled", Touchpad::isTappingEnabled()) ? Qt::Checked : Qt::Unchecked);
    ui->TappingTimeValueS->setValue(config.readEntry("TappingTime", Touchpad::tapTime()));
    ui->TappingDoubleTimeValueS->setValue(config.readEntry("DoubleTappingTime", Touchpad::doubleTapTime()));

    tappingButtonsMap[Synaptics::OneFinger] = config.readEntry("TappingEventOneFinger", (int)Touchpad::buttonForTap(Synaptics::OneFinger));
    tappingButtonsMap[Synaptics::TwoFingers] = config.readEntry("TappingEventTwoFingers", (int)Touchpad::buttonForTap(Synaptics::TwoFingers));
    tappingButtonsMap[Synaptics::ThreeFingers] = config.readEntry("TappingEventThreeFingers", (int)Touchpad::buttonForTap(Synaptics::ThreeFingers));
    tappingButtonsMap[Synaptics::RightTop] = config.readEntry("TappingEventRTCorner", (int)Touchpad::buttonForTap(Synaptics::RightTop));
    tappingButtonsMap[Synaptics::RightBottom] = config.readEntry("TappingEventRBCorner", (int)Touchpad::buttonForTap(Synaptics::RightBottom));
    tappingButtonsMap[Synaptics::LeftTop] = config.readEntry("TappingEventLTCorner", (int)Touchpad::buttonForTap(Synaptics::LeftTop));
    tappingButtonsMap[Synaptics::LeftBottom] = config.readEntry("TappingEventLBCorner", (int)Touchpad::buttonForTap(Synaptics::LeftBottom));
}

/*
 * Funkcja do zapisu konfiguracji touchpada do pliku.
 * Uruchamia ją przycisk "Zastosuj".
 * Wpierw wywołuje funkcję do ustawiania wartości w sterowniku,
 *      jeśli się ona nie powiedzie, to nie jest zapisywana konfiguracja.
 */
void TouchpadConfig::save()
{
    if(apply() == false)
        return;

    KConfigGroup config(KSharedConfig::openConfig("kcmtouchpadrc", KConfig::NoGlobals), "Touchpad");

    config.writeEntry("TouchpadOn", ui->TouchpadOnRB->isChecked());
    config.writeEntry("TouchpadAllowMoving", ui->TouchpadOffWOMoveCB->isChecked());

    //config.writeEntry("SmartModeEnabled", ui->SmartModeEnableCB->isChecked());
    //config.writeEntry("SmartModeDelay", ui->SmartModeDelayS->value());

    config.writeEntry("Sensitivity", ui->SensitivityValueS->value());

    config.writeEntry("ScrollingVerticalEnabled", ui->ScrollVertEnableCB->isChecked());
    config.writeEntry("ScrollingVerticalSpeed", ui->ScrollVertSpeedS->value());
    config.writeEntry("ScrollingVerticalTwoFingersEnabled", ui->ScrollVertTFEnableCB->isChecked());

    config.writeEntry("ScrollingHorizontalEnabled", ui->ScrollHorizEnableCB->isChecked());
    config.writeEntry("ScrollingHorizontalSpeed", ui->ScrollHorizSpeedS->value());
    config.writeEntry("ScrollingHorizontalTwoFingersEnabled", ui->ScrollHorizTFEnableCB->isChecked());

    config.writeEntry("ScrollingCoastingEnabled", ui->ScrollCoastingEnableCB->isChecked());
    config.writeEntry("ScrollingCoastingSpeed", ui->ScrollCoastingSpeedS->value());

    config.writeEntry("CircularScrollingEnabled", ui->ScrollCircularEnableCB->isChecked());
    config.writeEntry("CircularScrollingSpeed", ui->ScrollCircularSpeedS->value());
    config.writeEntry("CircularScrollingCorners", ui->ScrollCircularCornersCBB->currentIndex());

    
    config.writeEntry("TappingEnabled", ui->TappingEnableCB->isChecked());
    config.writeEntry("TappingTime", ui->TappingTimeValueS->value());
    config.writeEntry("DoubleTappingTime", ui->TappingDoubleTimeValueS->value());

    config.writeEntry("TappingEventOneFinger", this->tappingButtonsMap[Synaptics::OneFinger]);
    config.writeEntry("TappingEventTwoFingers", this->tappingButtonsMap[Synaptics::TwoFingers]);
    config.writeEntry("TappingEventThreeFingers", this->tappingButtonsMap[Synaptics::ThreeFingers]);
    config.writeEntry("TappingEventRTCorner", this->tappingButtonsMap[Synaptics::RightTop]);
    config.writeEntry("TappingEventRBCorner", this->tappingButtonsMap[Synaptics::RightBottom]);
    config.writeEntry("TappingEventLTCorner", this->tappingButtonsMap[Synaptics::LeftTop]);
    config.writeEntry("TappingEventLBCorner", this->tappingButtonsMap[Synaptics::LeftBottom]);

    // synchronizacja obiektu konfiguracyjnego i pliku na dysku
    config.sync();
}

/*
 * Funkcja wywoływana przez naciśnięcie przycisku "Domyślne".
 * Ustawia wartości domyślne dla parametrów sterownika.
 * Niezdefiniowana - wartości domyślne można uzyskać
 *      kasując plik z konfiguracją (~/.kde/share/config/kcmtouchpadrc)
 */
void TouchpadConfig::defaults()
{

}

/*
 * Funkcja krótkiej pomocy dla modułu.
 */
QString TouchpadConfig::quickHelp() const
{
  return QString("<h1>Touchpad</h1> This module allows you to choose options"
     " for the way in which your touchpad works. The actual effect of"
     " setting these options depends upon the features provided by your"
     " touchpad hardware and the X server on which KDE is running.");
}

/*
 * Funkcja zatwierdzająca zmiany poczynione w poszczególnych kontrolkach.
 * Pobiera wartość z każdej kontrolki i wywołuje odpowiadającą kontrolce
 *      funkcję zapisującą wartość do pamięci sterownika.
 */
bool TouchpadConfig::apply()
{
    if(!valid)
        return false;

    if(ui->TouchpadOffRB->isChecked())
    {
        if(ui->TouchpadOffWOMoveCB->isChecked())
            Touchpad::setTouchpadEnabled(2);
        else
            Touchpad::setTouchpadEnabled(1);
    }
    else
        Touchpad::setTouchpadEnabled(0);

    //Touchpad::setSmartModeEnabled(ui->SmartModeEnableCB->isChecked(), ui->SmartModeDelayS->value() / 1000);

    Touchpad::setSensitivity(ui->SensitivityValueS->value());

    Touchpad::setVerticalScrollingEnabled(ui->ScrollVertEnableCB->isChecked());
    Touchpad::setVerticalScrollingDelta(ui->ScrollVertSpeedS->value());
    Touchpad::setVerticalTwoFingersScrollingEnabled(ui->ScrollVertTFEnableCB->isChecked());

    Touchpad::setHorizontalScrollingEnabled(ui->ScrollHorizEnableCB->isChecked());
    Touchpad::setHorizontalScrollingDelta(ui->ScrollHorizSpeedS->value());
    Touchpad::setHorizontalTwoFingersScrollingEnabled(ui->ScrollHorizTFEnableCB->isChecked());

    Touchpad::setCoastingEnabled(ui->ScrollCoastingEnableCB->isChecked());
    Touchpad::setCoastingSpeed(ui->ScrollCoastingSpeedS->value());

    Touchpad::setCircularScrollingEnabled(ui->ScrollCircularEnableCB->isChecked());
    Touchpad::setCircularScrollingDelta(ui->ScrollCircularSpeedS->value());
    Touchpad::setCircularScrollingTrigger((ScrollTrigger)ui->ScrollCircularCornersCBB->currentIndex());

    Touchpad::setTappingEnabled(ui->TappingEnableCB->isChecked());
    Touchpad::setTapTime(ui->TappingTimeValueS->value());
    Touchpad::setDoubleTapTime((ui->TappingDoubleTimeValueS->value()));

    Touchpad::setButtonsForTap(Synaptics::OneFinger, (Synaptics::Button)this->tappingButtonsMap[Synaptics::OneFinger]);
    Touchpad::setButtonsForTap(Synaptics::TwoFingers, (Synaptics::Button)this->tappingButtonsMap[Synaptics::TwoFingers]);
    Touchpad::setButtonsForTap(Synaptics::ThreeFingers, (Synaptics::Button)this->tappingButtonsMap[Synaptics::ThreeFingers]);
    Touchpad::setButtonsForTap(Synaptics::RightTop, (Synaptics::Button)this->tappingButtonsMap[Synaptics::RightTop]);
    Touchpad::setButtonsForTap(Synaptics::RightBottom, (Synaptics::Button)this->tappingButtonsMap[Synaptics::RightBottom]);
    Touchpad::setButtonsForTap(Synaptics::LeftTop, (Synaptics::Button)this->tappingButtonsMap[Synaptics::LeftTop]);
    Touchpad::setButtonsForTap(Synaptics::LeftBottom, (Synaptics::Button)this->tappingButtonsMap[Synaptics::LeftBottom]);

    return true;
}



void TouchpadConfig::changed()
{
    emit KCModule::changed(true);
}

void TouchpadConfig::touchpadEnabled(bool toggle)
{
    emit this->changed();

    if(toggle)
        ui->TouchpadOffWOMoveCB->setEnabled(false);
    else
        ui->TouchpadOffWOMoveCB->setEnabled(true);
}

void TouchpadConfig::touchpadAllowedMoving(bool toggle)
{
    emit this->changed();
}

void TouchpadConfig::smartModeEnabled(bool toggle)
{
    /*emit this->changed();

    ui->SmartModeDelayL->setEnabled(toggle);
    ui->SmartModeDelayS->setEnabled(toggle);
    ui->SmartModeDelayValueL->setEnabled(toggle);
    ui->SmartModeDelayMilisecondsL->setEnabled(toggle);*/
}

void TouchpadConfig::smartModeDelayChanged(int value)
{
//    emit this->changed();
}

void TouchpadConfig::sensitivityValueChanged(int value)
{
    emit this->changed();
}

void TouchpadConfig::scrollVerticalEnabled(bool toggle)
{
    emit this->changed();

    ui->ScrollVertSlowL->setEnabled(toggle);
    ui->ScrollVertSpeedS->setEnabled(toggle);
    ui->ScrollVertFastL->setEnabled(toggle);
}

void TouchpadConfig::scrollVerticalSpeedChanged(int value)
{
    emit this->changed();
}

void TouchpadConfig::scrollVerticalTFEnabled(bool toggle)
{
    emit this->changed();
}

void TouchpadConfig::scrollHorizontalEnabled(bool toggle)
{
    emit this->changed();

    ui->ScrollHorizSlowL->setEnabled(toggle);
    ui->ScrollHorizSpeedS->setEnabled(toggle);
    ui->ScrollHorizFastL->setEnabled(toggle);
}

void TouchpadConfig::scrollHorizontalSpeedChanged(int value)
{
    emit this->changed();
}

void TouchpadConfig::scrollHorizontalTFEnabled(bool toggle)
{
    emit this->changed();
}

void TouchpadConfig::scrollCoastingEnabled(bool toggle)
{
    emit this->changed();

    ui->ScrollCoastingSpeedL->setEnabled(toggle);
    ui->ScrollCoastingSpeedS->setEnabled(toggle);
}

void TouchpadConfig::scrollCoastingSpeedChanged(int value)
{
    emit this->changed();
}

void TouchpadConfig::circularScrollEnabled(bool toggle)
{
    emit this->changed();

    ui->ScrollCircularSlowL->setEnabled(toggle);
    ui->ScrollCircularSpeedS->setEnabled(toggle);
    ui->ScrollCircularFastL->setEnabled(toggle);
    ui->ScrollCircularUseL->setEnabled(toggle);
    ui->ScrollCircularCornersCBB->setEnabled(toggle);
}

void TouchpadConfig::circularScrollSpeedChanged(int value)
{
    emit this->changed();
}

void TouchpadConfig::circularScrollCornersChosen(int chosen)
{
    emit this->changed();
}

void TouchpadConfig::tappingEnabled(bool toggle)
{
    emit this->changed();

    ui->TappingTimeL->setEnabled(toggle);
    ui->TappingTimeValueS->setEnabled(toggle);
    ui->TappingTimeValueL->setEnabled(toggle);
    ui->TappingTimeMilisecondsL->setEnabled(toggle);
    ui->TappingDoubleTimeL->setEnabled(toggle);
    ui->TappingDoubleTimeValueS->setEnabled(toggle);
    ui->TappingDoubleTimeValueL->setEnabled(toggle);
    ui->TappingDoubleTimeMilisecondsL->setEnabled(toggle);
}

void TouchpadConfig::tappingTimeChanged(int value)
{
    emit this->changed();
}

void TouchpadConfig::tappingDoubleTimeChanged(int value)
{
    emit this->changed();
}

void TouchpadConfig::tappingEventListSelected(int current)
{
    ui->TappingButtonLW->setCurrentRow(tappingButtonsMap[current]);
}

void TouchpadConfig::tappingButtonListSelected(int current)
{
    emit this->changed();

    tappingButtonsMap[ui->TappingEventLW->currentRow()] = current;
}

/*
 * Funkcja uruchamiana przez kcminit_startup podczas startowania KDE.
 * Służy do ustawiania wartości touchpada podczas uruchamiania.
 * Nie różni się wiele od funkcji apply().
 */
void TouchpadConfig::init_touchpad()
{
    KConfigGroup config(KSharedConfig::openConfig( "kcmtouchpadrc" ), "Touchpad");

    if(config.readEntry("TouchpadOn", !Touchpad::isTouchpadEnabled()))
        Touchpad::setTouchpadEnabled(0);
    else
    {
        if(config.readEntry("TouchpadAllowMoving", Touchpad::isTouchpadEnabled()))
            Touchpad::setTouchpadEnabled(2);
        else
            Touchpad::setTouchpadEnabled(1);
    }

//    Touchpad::setSmartModeEnabled(config.readEntry("SmartModeEnabled", Touchpad::isSmartModeEnabled()),
//                           config.readEntry("SmartModeDelay", 1000));

    Touchpad::setSensitivity(config.readEntry("Sensitivity", Touchpad::sensitivity()));

    Touchpad::setVerticalScrollingEnabled(config.readEntry("ScrollingVerticalEnabled", Touchpad::isVerticalScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);
    Touchpad::setVerticalScrollingDelta(config.readEntry("ScrollingVerticalSpeed", Touchpad::verticalScrollingDelta()));
    Touchpad::setVerticalTwoFingersScrollingEnabled(config.readEntry("ScrollingVerticalTwoFingersEnabled", Touchpad::isVerticalTwoFingersScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);

    Touchpad::setHorizontalScrollingEnabled(config.readEntry("ScrollingHorizontalEnabled", Touchpad::isHorizontalScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);
    Touchpad::setHorizontalScrollingDelta(config.readEntry("ScrollingHorizontalSpeed", Touchpad::horizontalScrollingDelta()));
    Touchpad::setHorizontalTwoFingersScrollingEnabled(config.readEntry("ScrollingHorizontalTwoFingersEnabled", Touchpad::isHorizontalTwoFingersScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);

    Touchpad::setCoastingEnabled(config.readEntry("ScrollingCoastingEnabled", Touchpad::isCoastingEnabled()) ? Qt::Checked : Qt::Unchecked);
    Touchpad::setCoastingSpeed(config.readEntry("ScrollingCoastingSpeed", Touchpad::coastingSpeed()));

    Touchpad::setCircularScrollingEnabled(config.readEntry("CircularScrollingEnabled", Touchpad::isCircularScrollingEnabled()) ? Qt::Checked : Qt::Unchecked);
    Touchpad::setCircularScrollingDelta(config.readEntry("CircularScrollingSpeed", Touchpad::circularScrollingDelta()));
    Touchpad::setCircularScrollingTrigger((Synaptics::ScrollTrigger)config.readEntry("CircularScrollingCorners", (int)Touchpad::circularScrollingTrigger()));

    Touchpad::setTappingEnabled(config.readEntry("TappingEnabled", Touchpad::isTappingEnabled()) ? Qt::Checked : Qt::Unchecked);
    Touchpad::setTapTime(config.readEntry("TappingTime", Touchpad::tapTime()));
    Touchpad::setDoubleTapTime(config.readEntry("DoubleTappingTime", Touchpad::doubleTapTime()));

    Touchpad::setButtonsForTap(Synaptics::OneFinger, (Synaptics::Button)config.readEntry("TappingEventOneFinger", (int)Touchpad::buttonForTap(Synaptics::OneFinger)));
    Touchpad::setButtonsForTap(Synaptics::TwoFingers, (Synaptics::Button)config.readEntry("TappingEventTwoFingers", (int)Touchpad::buttonForTap(Synaptics::TwoFingers)));
    Touchpad::setButtonsForTap(Synaptics::ThreeFingers, (Synaptics::Button)config.readEntry("TappingEventThreeFingers", (int)Touchpad::buttonForTap(Synaptics::ThreeFingers)));
    Touchpad::setButtonsForTap(Synaptics::RightTop, (Synaptics::Button)config.readEntry("TappingEventRTCorner", (int)Touchpad::buttonForTap(Synaptics::RightTop)));
    Touchpad::setButtonsForTap(Synaptics::RightBottom, (Synaptics::Button)config.readEntry("TappingEventRBCorner", (int)Touchpad::buttonForTap(Synaptics::RightBottom)));
    Touchpad::setButtonsForTap(Synaptics::LeftTop, (Synaptics::Button)config.readEntry("TappingEventLTCorner", (int)Touchpad::buttonForTap(Synaptics::LeftTop)));
    Touchpad::setButtonsForTap(Synaptics::LeftBottom, (Synaptics::Button)config.readEntry("TappingEventLBCorner", (int)Touchpad::buttonForTap(Synaptics::LeftBottom)));

}

extern "C"
{
    KDE_EXPORT void kcminit_touchpad()
    {
        TouchpadConfig::init_touchpad();
    }
}

#include "kcmtouchpad.moc"

