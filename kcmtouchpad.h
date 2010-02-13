/*
 * Copyright © 2009 Michał Żarłok
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the names of the authors
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  Authors
 * makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors: Michał Żarłok
 */

#ifndef _KCMTOUCHPAD_H
#define _KCMTOUCHPAD_H

#include <QSet>
#include <QString>
#include <QtDBus/QtDBus>

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
    static void applySensitivity(int val);
    static void setSmartMode(bool enable, unsigned interval);
    void enableProperties();

    Ui_TouchpadConfigWidget* ui;

    /* map events to button: (event) -> (button) */
    QMap<int, int> tappingButtonsMap;

    QSet<const char*> propertiesList;

    bool setup_failed;

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
    void scrollCoastingCornerEnabled(bool toggle);

    void circularScrollEnabled(bool toggle);
    void circularScrollSpeedChanged(int value);
    void circularScrollCornersChosen(int chosen);

    void tappingEnabled(bool toggle);
    void tappingMaxMoveChanged(int value);
    void tappingTimeoutChanged(int value);
    void tappingDoubleTimeChanged(int value);
    void tappingClickTimeChanged(int value);

    void tappingEventListSelected(int current);
    void tappingButtonListSelected(int current);
};

#endif

