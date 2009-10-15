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
#include <math.h>

#include "kcmtouchpad.h"
#include "ui_kcmtouchpadwidget.h"

#include "touchpad.h"

K_PLUGIN_FACTORY(TouchpadConfigFactory, registerPlugin<TouchpadConfig>("touchpad");)
K_EXPORT_PLUGIN(TouchpadConfigFactory("kcmtouchpad"))

TouchpadConfig::TouchpadConfig(QWidget *parent, const QVariantList &)
        : KCModule(TouchpadConfigFactory::componentData(), parent)
{
    if (Touchpad::init_xinput_extension()) {
        valid = 0;
        return;
    }

    const prop_list* properties_list = Touchpad::get_properties_list();
    for (prop_list::const_iterator it = properties_list->begin(); it != properties_list->end(); it++) {
        this->propertiesList.insert(*it);
    }

    // set user interface
    ui = new Ui_TouchpadConfigWidget();
    ui->setupUi(this);

    this->enableProperties();

    // we have to connect widgets to corresponding slots
    // "Touchpad On" radio button
    connect(ui->TouchpadOnRB, SIGNAL(toggled(bool)), this, SLOT(touchpadEnabled(bool)));
    // "Touchpad Allow Moving" check box
    connect(ui->TouchpadOffWOMoveCB, SIGNAL(toggled(bool)), this, SLOT(touchpadAllowedMoving(bool)));

    //connect(ui->SmartModeEnableCB, SIGNAL(toggled(bool)), this, SLOT(smartModeEnabled(bool)));
    //connect(ui->SmartModeDelayS, SIGNAL(valueChanged(int)), this, SLOT(smartModeDelayChanged(int)));

    // "Touch Sensitivity" slider
    connect(ui->SensitivityValueS, SIGNAL(valueChanged(int)), this, SLOT(sensitivityValueChanged(int)));

    // "Scrolling Vertical Enabled" check box
    connect(ui->ScrollVertEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollVerticalEnabled(bool)));
    // "Scrolling Vertical Speed" slider
    connect(ui->ScrollVertSpeedS, SIGNAL(valueChanged(int)), this, SLOT(scrollVerticalSpeedChanged(int)));
    // "Scrolling Vertical with Two Fingers Enabled" combo box
    connect(ui->ScrollVertTFEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollVerticalTFEnabled(bool)));

    // "Scrolling Horizontal Enabled" check box
    connect(ui->ScrollHorizEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollHorizontalEnabled(bool)));
    // "Scrolling Horizontal Speed" slider
    connect(ui->ScrollHorizSpeedS, SIGNAL(valueChanged(int)), this, SLOT(scrollHorizontalSpeedChanged(int)));
    // "Scrolling Horizontal with Two Fingers Enabled" check box
    connect(ui->ScrollHorizTFEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollHorizontalTFEnabled(bool)));

    // "Scroll Coasting Enabled" check box
    connect(ui->ScrollCoastingEnableCB, SIGNAL(toggled(bool)), this, SLOT(scrollCoastingEnabled(bool)));
    // "Scroll Coasting Speed" slider
    connect(ui->ScrollCoastingSpeedS, SIGNAL(valueChanged(int)), this, SLOT(scrollCoastingSpeedChanged(int)));

    // "Circular Scrolling Enabled" check box
    connect(ui->ScrollCircularEnableCB, SIGNAL(toggled(bool)), this, SLOT(circularScrollEnabled(bool)));
    // "Circular Scrolling Speed" slider
    connect(ui->ScrollCircularSpeedS, SIGNAL(valueChanged(int)), this, SLOT(circularScrollSpeedChanged(int)));
    // "Circular Scrolling Corners Trigger" combo box
    connect(ui->ScrollCircularCornersCBB, SIGNAL(currentIndexChanged(int)), this, SLOT(circularScrollCornersChosen(int)));

    // "Tapping Enabled" check box
    connect(ui->TappingEnableCB, SIGNAL(toggled(bool)), this, SLOT(tappingEnabled(bool)));
    // "Tapping Delay Timeout" slider
    connect(ui->TappingTimeoutValueS, SIGNAL(valueChanged(int)), this, SLOT(tappingTimeoutChanged(int)));
    // "Double Tapping Delay Time" slider
    connect(ui->TappingDoubleTimeValueS, SIGNAL(valueChanged(int)), this, SLOT(tappingDoubleTimeChanged(int)));
    // "Tapping Click Time" slider
    connect(ui->TappingClickTimeValueS, SIGNAL(valueChanged(int)), this, SLOT(tappingClickTimeChanged(int)));
    // "Tapping Event" list widget
    connect(ui->TappingEventLW, SIGNAL(currentRowChanged(int)), this, SLOT(tappingEventListSelected(int)));
    // "Corresponding Button" list widget
    connect(ui->TappingButtonLW, SIGNAL(currentRowChanged(int)), this, SLOT(tappingButtonListSelected(int)));

    valid = 1;
}

TouchpadConfig::~TouchpadConfig()
{
    Touchpad::free_xinput_extension();
}

void TouchpadConfig::enableProperties() {
    if (this->propertiesList.contains(SYNAPTICS_PROP_OFF)) {
        ui->TouchpadOnRB->setEnabled(TRUE);
        ui->TouchpadOffRB->setEnabled(TRUE);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_FINGER)) {
        ui->TouchSensitivityGB->setEnabled(TRUE);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_EDGE)) {
        ui->ScrollVertEnableCB->setEnabled(TRUE);
        ui->ScrollHorizEnableCB->setEnabled(TRUE);
        ui->ScrollCoastingEnableCB->setEnabled(TRUE);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_TWOFINGER)) {
        ui->ScrollVertTFEnableCB->setEnabled(TRUE);
        ui->ScrollHorizTFEnableCB->setEnabled(TRUE);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING)) {
        ui->ScrollCircularEnableCB->setEnabled(TRUE);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_TIME)) {
        ui->TappingEnableCB->setEnabled(TRUE);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_ACTION)) {
        ui->TappingButtonLW->setEnabled(TRUE);
        ui->TappingEventLW->setEnabled(TRUE);
        ui->ButtonTappingL->setEnabled(TRUE);
        ui->ButtonMeansL->setEnabled(TRUE);
    }
}

/*
 * This function is called when loading module.
 * It loads configuration from file "kcmtouchpadrc".
 */
void TouchpadConfig::load()
{
    if (!valid)
        return;

    KConfigGroup config(KSharedConfig::openConfig("kcmtouchpadrc", KConfig::NoGlobals), "Touchpad");

    // loads every entry of configuration and sets corresponding widget
    // when configuration doesn't exist collect actual value from driver

    if (this->propertiesList.contains(SYNAPTICS_PROP_OFF)) {
        ui->TouchpadOnRB->setChecked(!config.readEntry("TouchpadOff", !(int)*(char*)Touchpad::get_parameter("TouchpadOff")));
        ui->TouchpadOffWOMoveCB->setCheckState(config.readEntry("TouchpadOff", (int)*(char*)Touchpad::get_parameter("TouchpadOff")) == 2 ? Qt::Checked : Qt::Unchecked);
    }

    //ui->SmartModeEnableCB->setCheckState(config.readEntry("SmartModeEnabled", false) ? Qt::Checked : Qt::Unchecked);
    //ui->SmartModeDelayS->setValue(config.readEntry("SmartModeDelay", 1000));

//    ui->SensitivityValueS->setValue(config.readEntry("Sensitivity", Touchpad::sensitivity()));

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_EDGE)) {
        ui->ScrollVertEnableCB->setCheckState(config.readEntry("VertEdgeScroll", (int)*(char*)Touchpad::get_parameter("VertEdgeScroll")) ? Qt::Checked : Qt::Unchecked);
        ui->ScrollHorizEnableCB->setCheckState(config.readEntry("HorizEdgeScroll", (int)*(char*)Touchpad::get_parameter("HorizEdgeScroll")) ? Qt::Checked : Qt::Unchecked);
        ui->ScrollCoastingEnableCB->setCheckState(config.readEntry("CornerCoasting", (int)*(char*)Touchpad::get_parameter("CornerCoasting")) ? Qt::Checked : Qt::Unchecked);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_DISTANCE)) {
        ui->ScrollVertSpeedS->setValue(config.readEntry("VertScrollDelta", *(int*)Touchpad::get_parameter("VertScrollDelta")));
        ui->ScrollHorizSpeedS->setValue(config.readEntry("HorizScrollDelta", *(int*)Touchpad::get_parameter("HorizScrollDelta")));
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_TWOFINGER)) {
        ui->ScrollVertTFEnableCB->setCheckState(config.readEntry("VertTwoFingerScroll", (int)*(char*)Touchpad::get_parameter("VertTwoFingerScroll")) ? Qt::Checked : Qt::Unchecked);
        ui->ScrollHorizTFEnableCB->setCheckState(config.readEntry("HorizTwoFingerScroll", (int)*(char*)Touchpad::get_parameter("HorizTwoFingerScroll")) ? Qt::Checked : Qt::Unchecked);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_COASTING_SPEED)) {
        ui->ScrollCoastingSpeedS->setValue(config.readEntry("CoastingSpeed", *(double*)Touchpad::get_parameter("CoastingSpeed")));
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING)) {
        ui->ScrollCircularEnableCB->setCheckState(config.readEntry("CircularScrolling", (int)*(char*)Touchpad::get_parameter("CircularScrolling")) ? Qt::Checked : Qt::Unchecked);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_DIST)) {
        ui->ScrollCircularSpeedS->setValue(config.readEntry("CircScrollDelta", *(double*)Touchpad::get_parameter("CircScrollDelta")));
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_TRIGGER)) {
        ui->ScrollCircularCornersCBB->setCurrentIndex(config.readEntry("CircScrollTrigger", (int)*(char*)Touchpad::get_parameter("CircScrollTrigger")));
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_TIME)) {
        ui->TappingEnableCB->setCheckState(config.readEntry("MaxTapTime", *(int*)Touchpad::get_parameter("MaxTapTime")) ? Qt::Checked : Qt::Unchecked);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_DURATIONS)) {
        ui->TappingTimeoutValueS->setValue(config.readEntry("SingleTapTimeout", *(int*)Touchpad::get_parameter("SingleTapTimeout")));
        ui->TappingDoubleTimeValueS->setValue(config.readEntry("MaxDoubleTapTime", *(int*)Touchpad::get_parameter("MaxDoubleTapTime")));
        ui->TappingClickTimeValueS->setValue(config.readEntry("ClickTime", *(int*)Touchpad::get_parameter("ClickTime")));
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_ACTION)) {
        tappingButtonsMap[Synaptics::OneFinger] = config.readEntry("TapButton1", (int)*(char*)Touchpad::get_parameter("TapButton1"));
        tappingButtonsMap[Synaptics::TwoFingers] = config.readEntry("TapButton2", (int)*(char*)Touchpad::get_parameter("TapButton2"));
        tappingButtonsMap[Synaptics::ThreeFingers] = config.readEntry("TapButton3", (int)*(char*)Touchpad::get_parameter("TapButton3"));
        tappingButtonsMap[Synaptics::RightTop] = config.readEntry("RTCornerButton", (int)*(char*)Touchpad::get_parameter("RTCornerButton"));
        tappingButtonsMap[Synaptics::RightBottom] = config.readEntry("RBCornerButton", (int)*(char*)Touchpad::get_parameter("RBCornerButton"));
        tappingButtonsMap[Synaptics::LeftTop] = config.readEntry("LTCornerButton", (int)*(char*)Touchpad::get_parameter("LTCornerButton"));
        tappingButtonsMap[Synaptics::LeftBottom] = config.readEntry("LBCornerButton", (int)*(char*)Touchpad::get_parameter("LBCornerButton"));
    }
}

/*
 * This function at first tries to apply changes to driver,
 * and next, if it was successful, saves configuration to file.
 */
void TouchpadConfig::save()
{
    if (!valid)
        return;

    if(apply() == false)
        return;

    KConfigGroup config(KSharedConfig::openConfig("kcmtouchpadrc", KConfig::NoGlobals), "Touchpad");

    if (this->propertiesList.contains(SYNAPTICS_PROP_OFF)) {
        if (ui->TouchpadOffRB->isChecked()) {
            if (ui->TouchpadOffWOMoveCB->isChecked())
                config.writeEntry("TouchpadOff", 2);
            else
                config.writeEntry("TouchpadOff", 1);
        }
        else
            config.writeEntry("TouchpadOff", 0);
    }

    //config.writeEntry("SmartModeEnabled", ui->SmartModeEnableCB->isChecked());
    //config.writeEntry("SmartModeDelay", ui->SmartModeDelayS->value());

    //config.writeEntry("Sensitivity", ui->SensitivityValueS->value());

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_EDGE)) {
        config.writeEntry("VertEdgeScroll", (int)ui->ScrollVertEnableCB->isChecked());
        config.writeEntry("HorizEdgeScroll", (int)ui->ScrollHorizEnableCB->isChecked());
        config.writeEntry("CornerCoasting", (int)ui->ScrollCoastingEnableCB->isChecked());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_DISTANCE)) {
        config.writeEntry("VertScrollDelta", ui->ScrollVertSpeedS->value());
        config.writeEntry("HorizScrollDelta", ui->ScrollHorizSpeedS->value());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_TWOFINGER)) {
        config.writeEntry("VertTwoFingerScroll", (int)ui->ScrollVertTFEnableCB->isChecked());
        config.writeEntry("HorizTwoFingerScroll", (int)ui->ScrollHorizTFEnableCB->isChecked());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_COASTING_SPEED)) {
        config.writeEntry("CoastingSpeed", (double)ui->ScrollCoastingSpeedS->value());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING)) {
        config.writeEntry("CircularScrolling", (int)ui->ScrollCircularEnableCB->isChecked());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_DIST)) {
        config.writeEntry("CircScrollDelta", (double)ui->ScrollCircularSpeedS->value());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_TRIGGER)) {
        config.writeEntry("CircScrollTrigger", ui->ScrollCircularCornersCBB->currentIndex());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_TIME)) {
        config.writeEntry("MaxTapTime", (int)ui->TappingEnableCB->isChecked() * 180);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_DURATIONS)) {
        config.writeEntry("SingleTapTimeout", ui->TappingTimeoutValueS->value());
        config.writeEntry("MaxDoubleTapTime", ui->TappingDoubleTimeValueS->value());
        config.writeEntry("ClickTime", ui->TappingClickTimeValueS->value());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_ACTION)) {
        config.writeEntry("TapButton1", this->tappingButtonsMap[Synaptics::OneFinger]);
        config.writeEntry("TapButton2", this->tappingButtonsMap[Synaptics::TwoFingers]);
        config.writeEntry("TapButton3", this->tappingButtonsMap[Synaptics::ThreeFingers]);
        config.writeEntry("RTCornerButton", this->tappingButtonsMap[Synaptics::RightTop]);
        config.writeEntry("RBCornerButton", this->tappingButtonsMap[Synaptics::RightBottom]);
        config.writeEntry("LTCornerButton", this->tappingButtonsMap[Synaptics::LeftTop]);
        config.writeEntry("LBCornerButton", this->tappingButtonsMap[Synaptics::LeftBottom]);
    }

    // synchronize config entries with file
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
 * Returns quick help
 */
QString TouchpadConfig::quickHelp() const
{
  return QString("<h1>Touchpad</h1> This module allows you to choose options"
     " for the way in which your touchpad works. The actual effect of"
     " setting these options depends upon the features provided by your"
     " touchpad hardware and the X server on which KDE is running.");
}

/*
 * This function applies changes to driver.
 * It gets value from every widget and calls corresponding function
 * which saves this value to SHM
*/
bool TouchpadConfig::apply()
{
    if (this->propertiesList.contains(SYNAPTICS_PROP_OFF)) {
        if(ui->TouchpadOffRB->isChecked())
        {
            if(ui->TouchpadOffWOMoveCB->isChecked())
                Touchpad::set_parameter("TouchpadOff", 2);
            else
                Touchpad::set_parameter("TouchpadOff", 1);
        }
        else
            Touchpad::set_parameter("TouchpadOff", 0);
    }

    //Touchpad::setSmartModeEnabled(ui->SmartModeEnableCB->isChecked(), ui->SmartModeDelayS->value() / 1000);

//    Touchpad::setSensitivity(ui->SensitivityValueS->value());

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_EDGE)) {
        Touchpad::set_parameter("VertEdgeScroll", ui->ScrollVertEnableCB->isChecked());
        Touchpad::set_parameter("HorizEdgeScroll", ui->ScrollHorizEnableCB->isChecked());
        Touchpad::set_parameter("CornerCoasting", ui->ScrollCoastingEnableCB->isChecked());
    }

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_DISTANCE)) {
        Touchpad::set_parameter("VertScrollDelta", ui->ScrollVertSpeedS->value());
        Touchpad::set_parameter("HorizScrollDelta", ui->ScrollHorizSpeedS->value());
    }

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_TWOFINGER)) {
        Touchpad::set_parameter("VertTwoFingerScroll", ui->ScrollVertTFEnableCB->isChecked());
        Touchpad::set_parameter("HorizTwoFingerScroll", ui->ScrollHorizTFEnableCB->isChecked());
    }

    if (this->propertiesList.contains(SYNAPTICS_PROP_COASTING_SPEED)) {
        Touchpad::set_parameter("CoastingSpeed", ui->ScrollCoastingSpeedS->value());
    }

    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING)) {
        Touchpad::set_parameter("CircularScrolling", ui->ScrollCircularEnableCB->isChecked());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_DIST)) {
        Touchpad::set_parameter("CircScrollDelta", ui->ScrollCircularSpeedS->value());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_TRIGGER)) {
        Touchpad::set_parameter("CircScrollTrigger", (Synaptics::ScrollTrigger)ui->ScrollCircularCornersCBB->currentIndex());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_TIME)) {
        Touchpad::set_parameter("MaxTapTime", (int)ui->TappingEnableCB->isChecked() * 180);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_DURATIONS)) {
        Touchpad::set_parameter("SingleTapTimeout", ui->TappingTimeoutValueS->value());
        Touchpad::set_parameter("MaxDoubleTapTime", ui->TappingDoubleTimeValueS->value());
        Touchpad::set_parameter("ClickTime", ui->TappingClickTimeValueS->value());
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_ACTION)) {
        Touchpad::set_parameter("TapButton1", (Synaptics::Button)this->tappingButtonsMap[Synaptics::OneFinger]);
        Touchpad::set_parameter("TapButton2", (Synaptics::Button)this->tappingButtonsMap[Synaptics::TwoFingers]);
        Touchpad::set_parameter("TapButton3", (Synaptics::Button)this->tappingButtonsMap[Synaptics::ThreeFingers]);
        Touchpad::set_parameter("RTCornerButton", (Synaptics::Button)this->tappingButtonsMap[Synaptics::RightTop]);
        Touchpad::set_parameter("RBCornerButton", (Synaptics::Button)this->tappingButtonsMap[Synaptics::RightBottom]);
        Touchpad::set_parameter("LTCornerButton", (Synaptics::Button)this->tappingButtonsMap[Synaptics::LeftTop]);
        Touchpad::set_parameter("LBCornerButton", (Synaptics::Button)this->tappingButtonsMap[Synaptics::LeftBottom]);
    }

    return true;
}



void TouchpadConfig::changed() {
    emit KCModule::changed(true);
}

void TouchpadConfig::touchpadEnabled(bool toggle) {
    emit this->changed();

    if(toggle)
        ui->TouchpadOffWOMoveCB->setEnabled(false);
    else
        ui->TouchpadOffWOMoveCB->setEnabled(true);
}

void TouchpadConfig::touchpadAllowedMoving(bool toggle) {
    emit this->changed();
}

void TouchpadConfig::smartModeEnabled(bool toggle) {
    /*emit this->changed();

    ui->SmartModeDelayL->setEnabled(toggle);
    ui->SmartModeDelayS->setEnabled(toggle);
    ui->SmartModeDelayValueL->setEnabled(toggle);
    ui->SmartModeDelayMilisecondsL->setEnabled(toggle);*/
}

void TouchpadConfig::smartModeDelayChanged(int value) {
//    emit this->changed();
}

void TouchpadConfig::sensitivityValueChanged(int value) {
    emit this->changed();
}

void TouchpadConfig::scrollVerticalEnabled(bool toggle) {
    emit this->changed();

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_DISTANCE)) {
        ui->ScrollVertHighL->setEnabled(ui->ScrollVertEnableCB->isChecked() || ui->ScrollVertTFEnableCB->isChecked());
        ui->ScrollVertSpeedS->setEnabled(ui->ScrollVertEnableCB->isChecked() || ui->ScrollVertTFEnableCB->isChecked());
        ui->ScrollVertLowL->setEnabled(ui->ScrollVertEnableCB->isChecked() || ui->ScrollVertTFEnableCB->isChecked());
    }
}

void TouchpadConfig::scrollVerticalSpeedChanged(int value) {
    emit this->changed();
}

void TouchpadConfig::scrollVerticalTFEnabled(bool toggle) {
    emit this->changed();

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_DISTANCE)) {
        ui->ScrollVertHighL->setEnabled(ui->ScrollVertEnableCB->isChecked() || ui->ScrollVertTFEnableCB->isChecked());
        ui->ScrollVertSpeedS->setEnabled(ui->ScrollVertEnableCB->isChecked() || ui->ScrollVertTFEnableCB->isChecked());
        ui->ScrollVertLowL->setEnabled(ui->ScrollVertEnableCB->isChecked() || ui->ScrollVertTFEnableCB->isChecked());
    }
}

void TouchpadConfig::scrollHorizontalEnabled(bool toggle) {
    emit this->changed();

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_DISTANCE)) {
        ui->ScrollHorizHighL->setEnabled(ui->ScrollHorizEnableCB->isChecked() || ui->ScrollHorizTFEnableCB->isChecked());
        ui->ScrollHorizSpeedS->setEnabled(ui->ScrollHorizEnableCB->isChecked() || ui->ScrollHorizTFEnableCB->isChecked());
        ui->ScrollHorizLowL->setEnabled(ui->ScrollHorizEnableCB->isChecked() || ui->ScrollHorizTFEnableCB->isChecked());
    }
}

void TouchpadConfig::scrollHorizontalSpeedChanged(int value) {
    emit this->changed();
}

void TouchpadConfig::scrollHorizontalTFEnabled(bool toggle) {
    emit this->changed();

    if (this->propertiesList.contains(SYNAPTICS_PROP_SCROLL_DISTANCE)) {
        ui->ScrollHorizHighL->setEnabled(ui->ScrollHorizEnableCB->isChecked() || ui->ScrollHorizTFEnableCB->isChecked());
        ui->ScrollHorizSpeedS->setEnabled(ui->ScrollHorizEnableCB->isChecked() || ui->ScrollHorizTFEnableCB->isChecked());
        ui->ScrollHorizLowL->setEnabled(ui->ScrollHorizEnableCB->isChecked() || ui->ScrollHorizTFEnableCB->isChecked());
    }
}

void TouchpadConfig::scrollCoastingEnabled(bool toggle) {
    emit this->changed();

    if (this->propertiesList.contains(SYNAPTICS_PROP_COASTING_SPEED)) {
        ui->ScrollCoastingSlowL->setEnabled(toggle);
        ui->ScrollCoastingFastL->setEnabled(toggle);
        ui->ScrollCoastingSpeedS->setEnabled(toggle);
    }
}

void TouchpadConfig::scrollCoastingSpeedChanged(int value) {
    emit this->changed();
}

void TouchpadConfig::circularScrollEnabled(bool toggle) {
    emit this->changed();

    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_DIST)) {
        ui->ScrollCircularSlowL->setEnabled(toggle);
        ui->ScrollCircularSpeedS->setEnabled(toggle);
        ui->ScrollCircularFastL->setEnabled(toggle);
        ui->ScrollCircularUseL->setEnabled(toggle);
    }
    if (this->propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_TRIGGER)) {
        ui->ScrollCircularCornersCBB->setEnabled(toggle);
    }
}

void TouchpadConfig::circularScrollSpeedChanged(int value) {
    emit this->changed();
}

void TouchpadConfig::circularScrollCornersChosen(int chosen) {
    emit this->changed();
}

void TouchpadConfig::tappingEnabled(bool toggle) {
    emit this->changed();

    if (this->propertiesList.contains(SYNAPTICS_PROP_TAP_DURATIONS)) {
        ui->TappingTimeoutL->setEnabled(toggle);
        ui->TappingTimeoutValueS->setEnabled(toggle);
        ui->TappingTimeoutValueL->setEnabled(toggle);
        ui->TappingTimeoutMilisecondsL->setEnabled(toggle);
        ui->TappingDoubleTimeL->setEnabled(toggle);
        ui->TappingDoubleTimeValueS->setEnabled(toggle);
        ui->TappingDoubleTimeValueL->setEnabled(toggle);
        ui->TappingDoubleTimeMilisecondsL->setEnabled(toggle);
        ui->TappingClickTimeL->setEnabled(toggle);
        ui->TappingClickTimeValueS->setEnabled(toggle);
        ui->TappingClickTimeValueL->setEnabled(toggle);
        ui->TappingClickTimeMillisecondsL->setEnabled(toggle);
    }
}

void TouchpadConfig::tappingTimeoutChanged(int value) {
    emit this->changed();
}

void TouchpadConfig::tappingDoubleTimeChanged(int value) {
    emit this->changed();
}

void TouchpadConfig::tappingClickTimeChanged(int value) {
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
 * Function called at KDE startup.
 * Loads saved configuration and applies it to driver.
 */
void TouchpadConfig::init_touchpad()
{
    if (Touchpad::init_xinput_extension()) {
        return;
    }
    KConfigGroup config(KSharedConfig::openConfig( "kcmtouchpadrc" ), "Touchpad");

    QList<const char*> propertiesList;
    const prop_list* properties_list = Touchpad::get_properties_list();
    for (prop_list::const_iterator it = properties_list->begin(); it != properties_list->end(); it++) {
        propertiesList.append(*it);
    }

    if (propertiesList.contains(SYNAPTICS_PROP_OFF)) {
        Touchpad::set_parameter("TouchpadOff", config.readEntry("TouchpadOff", (int)*(char*)Touchpad::get_parameter("TouchpadOff")));
    }

//    Touchpad::setSmartModeEnabled(config.readEntry("SmartModeEnabled", Touchpad::isSmartModeEnabled()),
//                           config.readEntry("SmartModeDelay", 1000));

//    Touchpad::setSensitivity(config.readEntry("Sensitivity", Touchpad::get_parameter());

    if (propertiesList.contains(SYNAPTICS_PROP_SCROLL_EDGE)) {
        Touchpad::set_parameter("VertEdgeScroll", config.readEntry("VertEdgeScroll", (int)*(char*)Touchpad::get_parameter("VertEdgeScroll")) ? Qt::Checked : Qt::Unchecked);
        Touchpad::set_parameter("HorizEdgeScroll", config.readEntry("HorizEdgeScroll", (int)*(char*)Touchpad::get_parameter("HorizEdgeScroll")) ? Qt::Checked : Qt::Unchecked);
    }
    if (propertiesList.contains(SYNAPTICS_PROP_SCROLL_DISTANCE)) {
        Touchpad::set_parameter("VertScrollDelta", config.readEntry("VertScrollDelta", *(int*)Touchpad::get_parameter("VertScrollDelta")));
        Touchpad::set_parameter("HorizScrollDelta", config.readEntry("HorizScrollDelta", *(int*)Touchpad::get_parameter("HorizScrollDelta")));
        Touchpad::set_parameter("CornerCoasting", config.readEntry("CornerCoasting", (int)*(char*)Touchpad::get_parameter("CornerCoasting")) ? Qt::Checked : Qt::Unchecked);
    }
    if (propertiesList.contains(SYNAPTICS_PROP_SCROLL_TWOFINGER)) {
        Touchpad::set_parameter("VertTwoFingerScroll", config.readEntry("VertTwoFingerScroll", (int)*(char*)Touchpad::get_parameter("VertTwoFingerScroll")) ? Qt::Checked : Qt::Unchecked);
        Touchpad::set_parameter("HorizTwoFingerScroll", config.readEntry("HorizTwoFingerScroll", (int)*(char*)Touchpad::get_parameter("HorizTwoFingerScroll")) ? Qt::Checked : Qt::Unchecked);
    }
    if (propertiesList.contains(SYNAPTICS_PROP_COASTING_SPEED)) {
        Touchpad::set_parameter("CoastingSpeed", config.readEntry("CoastingSpeed", *(double*)Touchpad::get_parameter("CoastingSpeed")));
    }
    if (propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING)) {
        Touchpad::set_parameter("CircularScrolling", config.readEntry("CircularScrolling", (int)*(char*)Touchpad::get_parameter("CircularScrolling")) ? Qt::Checked : Qt::Unchecked);
    }
    if (propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_DIST)) {
        Touchpad::set_parameter("CircScrollDelta", config.readEntry("CircScrollDelta", *(double*)Touchpad::get_parameter("CircScrollDelta")));
    }
    if (propertiesList.contains(SYNAPTICS_PROP_CIRCULAR_SCROLLING_TRIGGER)) {
        Touchpad::set_parameter("CircScrollTrigger", (Synaptics::ScrollTrigger)config.readEntry("CircScrollTrigger", (int)*(char*)Touchpad::get_parameter("CircScrollTrigger")));
    }
    if (propertiesList.contains(SYNAPTICS_PROP_TAP_DURATIONS)) {
        Touchpad::set_parameter("MaxTapTime", config.readEntry("MaxTapTime", *(int*)Touchpad::get_parameter("MaxTapTime")) ? Qt::Checked : Qt::Unchecked);
        Touchpad::set_parameter("SingleTapTimeout", config.readEntry("SingleTapTimeout", *(int*)Touchpad::get_parameter("SingleTapTimeout")));
        Touchpad::set_parameter("MaxDoubleTapTime", config.readEntry("MaxDoubleTapTime", *(int*)Touchpad::get_parameter("MaxDoubleTapTime")));
        Touchpad::set_parameter("ClickTime", config.readEntry("ClickTime", *(int*)Touchpad::get_parameter("ClickTime")));
    }
    if (propertiesList.contains(SYNAPTICS_PROP_TAP_ACTION)) {
        Touchpad::set_parameter("TapButton1", (Synaptics::Button)config.readEntry("TapButton1", (int)*(char*)Touchpad::get_parameter("TapButton1")));
        Touchpad::set_parameter("TapButton2", (Synaptics::Button)config.readEntry("TapButton2", (int)*(char*)Touchpad::get_parameter("TapButton2")));
        Touchpad::set_parameter("TapButton3", (Synaptics::Button)config.readEntry("TapButton3", (int)*(char*)Touchpad::get_parameter("TapButton3")));
        Touchpad::set_parameter("RTCornerButton", (Synaptics::Button)config.readEntry("RTCornerButton", (int)*(char*)Touchpad::get_parameter("RTCornerButton")));
        Touchpad::set_parameter("RBCornerButton", (Synaptics::Button)config.readEntry("RBCornerButton", (int)*(char*)Touchpad::get_parameter("RBCornerButton")));
        Touchpad::set_parameter("LTCornerButton", (Synaptics::Button)config.readEntry("LTCornerButton", (int)*(char*)Touchpad::get_parameter("LTCornerButton")));
        Touchpad::set_parameter("LBCornerButton", (Synaptics::Button)config.readEntry("LBCornerButton", (int)*(char*)Touchpad::get_parameter("LBCornerButton")));
    }

    Touchpad::free_xinput_extension();
}

extern "C"
{
    KDE_EXPORT void kcminit_touchpad()
    {
        TouchpadConfig::init_touchpad();
    }
}

#include "kcmtouchpad.moc"

