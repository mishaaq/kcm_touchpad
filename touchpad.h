#ifndef _TOUCHPAD_H
#define	_TOUCHPAD_H

#include <synaptics/synaptics.h>

using namespace Synaptics;

// singleton biblioteki Libsynaptics
static Pad* device = Pad::getInstance();

/*
 * Przestrzeń nazw dla funkcji komunikujących się ze sterownikiem touchpada.
 */
namespace Touchpad
{
    // pobiera wersję sterownika
    QString driverVersion();
    // pobiera wersję biblioteki libsynaptics
    QString libraryVersion();

    // czy touchpad jest włączony, wyłączony, bez tapnięć
    int isTouchpadEnabled();
    // czy włączona jest funkcja blokowania touchpada podczas pisania
    bool isSmartModeEnabled();

    // wartość czułości dotyku
    int sensitivity();

    // położenie palca na touchpadzie
    int absCoordX();
    int absCoordY();

    // prostokąt ograniczający pola przewijania
    // uwaga: określa miejsce na touchpadzie, gdzie NIE występuje pole przewijania
    //      (jest otoczony polami przewijania)
    QRect edges();

    // czy tapnięcia są włączone
    bool isTappingEnabled();
    // czas dotyku touchpada, żeby było to odebrane jako tapnięcie
    int tapTime();
    // czas dwóch dotyków touchpada, żeby było to odebrane jako podwójne tapnięcie
    int doubleTapTime();
    // zwraca przyporządkowany klawisz myszy do podanego działania
    // TapType może być: tapnięcie jednym palcem, dwoma palcami, trzema
    //      tapnięcie w rogach touchpada
    Button buttonForTap(TapType tap);
    // czy właściwość Fast Taps jest włączona
    bool areFastTapsEnabled();

    // czy przewijanie pionowe po prawym polu przewijania jest włączone
    bool isVerticalScrollingEnabled();
    // czy przewijanie pionowe dwoma palcami jest włączone
    bool isVerticalTwoFingersScrollingEnabled();
    // szybkość przewijania pionowego
    int verticalScrollingDelta();
    // czy przewijanie poziome po dolnym polu przewijania jest włączone
    bool isHorizontalScrollingEnabled();
    // czy przewijanie poziome dwoma palcami jest włączone
    bool isHorizontalTwoFingersScrollingEnabled();
    // szybkość przewijania poziomego
    int horizontalScrollingDelta();
    // czy właściwość Coasting jest włączona
    // Coasting pozwala na ciągłe przewijanie, gdy palec podczas przeciągnięcia
    //      po polu przewijania zatrzyma się w rogu
    bool isCoastingEnabled();
    // czybkość przewijania dla funkcji Coasting
    int coastingSpeed();
    // czy jest włączone przewijanie kołowe
    bool isCircularScrollingEnabled();
    // szybkość przewiajania kołowego
    int circularScrollingDelta();
    // miejsce na touchpadzie, które inicjuje przewijanie kołowe
    ScrollTrigger circularScrollingTrigger();

    //bool isEdgeMotionEnabled();


    void setTouchpadEnabled(const int type);
    void setSmartModeEnabled(bool enabled, const int time);
    void setSensitivity(const int value);
    
    void setEdges(const QRect &rect);

    void setTappingEnabled(bool enable); // workaround
    void setTapTime(int time);
    void setDoubleTapTime(int time);
    void setButtonsForTap(const TapType type, Button button);
    void setFastTapsEnabled(bool enable);

    void setVerticalScrollingEnabled(bool enable);
    void setVerticalTwoFingersScrollingEnabled(bool enable);
    void setVerticalScrollingDelta(int delta);
    void setHorizontalScrollingEnabled(bool enable);
    void setHorizontalTwoFingersScrollingEnabled(bool enable);
    void setHorizontalScrollingDelta(int delta);
    void setCoastingEnabled(bool enable);
    void setCoastingSpeed(int speed);
    void setCircularScrollingEnabled(bool enable);
    void setCircularScrollingDelta(int delta);
    void setCircularScrollingTrigger(ScrollTrigger trigger);

    //void setEdgeMotionEnabled(bool enable);
}

#endif	/* _TOUCHPAD_H */
