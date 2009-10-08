#include <QString>
#include <QRect>

#include <synaptics/pad.h>

#include "touchpad.h"

using namespace Synaptics;

static int finger_low[] = {53, 38, 25, 18, 11};


namespace Touchpad
{
    QString driverVersion()
    {
        return QString::fromStdString(device->driverStr());
    }

    QString libraryVersion()
    {
        return QString::fromStdString(device->libraryStr());
    }

    int isTouchpadEnabled()
    {
        return device->getParam(TOUCHPADOFF);
    }

    bool isSmartModeEnabled()
    {
        return device->isSynDaemonRunning();
    }

    int sensitivity()
    {
        int i;
        for(i = 0; i < 5; i++)
        {
            if(device->getParam(FINGERLOW) >= finger_low[i])
                return i;
        }

        return i - 1;
    }

    int absCoordX()
    {
        return (int)device->getParam(ABSCOORDX);
    }

    int absCoordY()
    {
        return (int)device->getParam(ABSCOORDY);
    }

    QRect edges()
    {
        return QRect(QPoint((int)device->getParam(LEFTEDGE),
                            (int)device->getParam(TOPEDGE)),
                     QPoint((int)device->getParam(RIGHTEDGE),
                            (int)device->getParam(BOTTOMEDGE)));
    }

    bool isTappingEnabled()
    {
        return device->getParam(MAXTAPTIME) > 0;
    }

    int tapTime()
    {
        return device->getParam(MAXTAPTIME);
    }

    int doubleTapTime()
    {
        return device->getParam(MAXDOUBLETAPTIME);
    }

    Button buttonForTap(TapType type)
    {
        Button button;
        switch(type)
        {
            case RightTop:
                button = (Button)device->getParam(RTCORNERBUTTON);
                break;
            case RightBottom:
                button = (Button)device->getParam(RBCORNERBUTTON);
                break;
            case LeftTop:
                button = (Button)device->getParam(LTCORNERBUTTON);
                break;
            case LeftBottom:
                button = (Button)device->getParam(LBCORNERBUTTON);
                break;
            case OneFinger:
                button = (Button)device->getParam(TAPBUTTON1);
                break;
            case TwoFingers:
                button = (Button)device->getParam(TAPBUTTON2);
                break;
            case ThreeFingers:
                button = (Button)device->getParam(TAPBUTTON3);
                break;
            default:
                button = NoButton;
        }

        return button;
    }

    bool areFastTapsEnabled()
    {
        return (bool)device->getParam(FASTTAPS);
    }

    bool isVerticalScrollingEnabled()
    {
        return device->getParam(VERTEDGESCROLL) > 0;
    }

    bool isVerticalTwoFingersScrollingEnabled()
    {
        return (bool)device->getParam(VERTTWOFINGERSCROLL);
    }

    int verticalScrollingDelta()
    {
        return (int)device->getParam(VERTSCROLLDELTA);
    }

    bool isHorizontalScrollingEnabled()
    {
        return device->getParam(HORIZEDGESCROLL) > 0;
    }

    bool isHorizontalTwoFingersScrollingEnabled()
    {
        return (bool)device->getParam(HORIZTWOFINGERSCROLL);
    }

    int horizontalScrollingDelta()
    {
        return (int)device->getParam(HORIZSCROLLDELTA);
    }

    bool isCoastingEnabled()
    {
        return device->getParam(CORNERCOASTING);
    }

    int coastingSpeed()
    {
        return device->getParam(COASTINGSPEED) * 2;
    }

    bool isCircularScrollingEnabled()
    {
        return (bool)device->getParam(CIRCULARSCROLLING);
    }

    int circularScrollingDelta()
    {
        return (int)device->getParam(CIRCSCROLLDELTA) * 100;
    }

    ScrollTrigger circularScrollingTrigger()
    {
        return (ScrollTrigger)device->getParam(CIRCSCROLLTRIGGER);
    }

    bool isEdgeMotionEnabled()
    {
        return (bool)device->getParam(EDGEMOTIONUSEALWAYS);
    }


    

    void setTouchpadEnabled(const int type)
    {
        device->setParam(TOUCHPADOFF, (double)type);
    }

    void setSmartModeEnabled(bool enabled, const int time)
    {
        //enabled ? device->runSynDaemon(time) : device->killSynDaemonProcesses();
    }
    
    void setSensitivity(const int value)
    {
        if(value < 0 || value > 4)
            return;

        device->setParam(FINGERLOW, finger_low[value]);
        device->setParam(FINGERHIGH, finger_low[value] + 5);
    }

    void setEdges(const QRect &rect)
    {
        device->setParam(TOPEDGE, rect.top());
        device->setParam(RIGHTEDGE, rect.right());
        device->setParam(BOTTOMEDGE, rect.bottom());
        device->setParam(LEFTEDGE, rect.left());
    }

    void setTappingEnabled(bool enable)
    {
        device->setParam(MAXTAPTIME, enable ? 180 : 0);
    }

    void setTapTime(int time)
    {
        device->setParam(MAXTAPTIME, (double)time);
    }

    void setDoubleTapTime(int time)
    {
        device->setParam(MAXDOUBLETAPTIME, (double)time);
    }

    void setButtonsForTap(const TapType type, Button button)
    {
        switch(type)
        {
            case RightTop:
                device->setParam(RTCORNERBUTTON, button);
                break;
            case LeftTop:
                device->setParam(LTCORNERBUTTON, button);
                break;
            case RightBottom:
                device->setParam(RBCORNERBUTTON, button);
                break;
            case LeftBottom:
                device->setParam(LBCORNERBUTTON, button);
                break;
            case OneFinger:
                device->setParam(TAPBUTTON1, button);
                break;
            case TwoFingers:
                device->setParam(TAPBUTTON2, button);
                break;
            case ThreeFingers:
                device->setParam(TAPBUTTON3, button);
                break;
            default:
                break;
        }
    }

    void setFastTapsEnabled(bool enable)
    {
        device->setParam(FASTTAPS, (double)enable);
    }

    void setVerticalScrollingEnabled(bool enable)
    {
        device->setParam(VERTEDGESCROLL, (double)enable);
    }

    void setVerticalTwoFingersScrollingEnabled(bool enable)
    {
        device->setParam(VERTTWOFINGERSCROLL, (double)enable);
    }

    void setVerticalScrollingDelta(int delta)
    {
        device->setParam(VERTSCROLLDELTA, delta);
    }

    void setHorizontalScrollingEnabled(bool enable)
    {
        device->setParam(HORIZEDGESCROLL, (double)enable);
    }

    void setHorizontalTwoFingersScrollingEnabled(bool enable)
    {
        device->setParam(HORIZTWOFINGERSCROLL, (double)enable);
    }

    void setHorizontalScrollingDelta(int delta)
    {
        device->setParam(HORIZSCROLLDELTA, delta);
    }

    void setCoastingEnabled(bool enable)
    {
        device->setParam(CORNERCOASTING, (double)enable);
    }

    void setCoastingSpeed(int speed)
    {
        device->setParam(COASTINGSPEED, (double)speed / 2);
    }

    void setCircularScrollingEnabled(bool enable)
    {
        device->setParam(CIRCULARSCROLLING, enable);
    }

    void setCircularScrollingDelta(int delta)
    {
        device->setParam(CIRCSCROLLDELTA, delta * 0.01);
    }

    void setCircularScrollingTrigger(ScrollTrigger trigger)
    {
        device->setParam(CIRCSCROLLTRIGGER, (double)trigger);
    }

    void setEdgeMotionEnabled(bool enable)
    {
        device->setParam(EDGEMOTIONUSEALWAYS, enable);
    }
}
