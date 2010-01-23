/*
 * Copyright © 2008 Red Hat, Inc.
 * Copyright © 2009 Michał Żarłok
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Red Hat
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  Red
 * Hat makes no representations about the suitability of this software
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
 * Authors: Peter Hutterer
 *          Michał Żarłok
 */

#ifndef _TOUCHPAD_H
#define	_TOUCHPAD_H

#include <stdio.h>
#include <stdlib.h>
#include <list>

#include "synaptics-properties.h"

#define GET_DISPLAY_FAILED -1
#define GET_DEVICE_FAILED -2


#ifndef XATOM_FLOAT
#define XATOM_FLOAT "FLOAT"
#endif

#define SYN_MAX_BUTTONS 12

#define SBR_MIN 10
#define SBR_MAX 1000

typedef std::list<const char*> prop_list;

union flong { /* Xlibs 64-bit property handling madness */
    long l;
    float f;
};

enum ParaType {
    PT_INT,
    PT_BOOL,
    PT_DOUBLE
};

struct Parameter {
    const char *name;                       /* Name of parameter */
    enum ParaType type;			    /* Type of parameter */
    double min_val;			    /* Minimum allowed value */
    double max_val;			    /* Maximum allowed value */
    const char *prop_name;                  /* Property name */
    int prop_format;			    /* Property format (0 for floats) */
    int prop_offset;			    /* Offset inside property */
};

static struct Parameter params[] = {
    {"LeftEdge",              PT_INT,    0, 10000, SYNAPTICS_PROP_EDGES,	32,	0},
    {"RightEdge",             PT_INT,    0, 10000, SYNAPTICS_PROP_EDGES,	32,	1},
    {"TopEdge",               PT_INT,    0, 10000, SYNAPTICS_PROP_EDGES,	32,	2},
    {"BottomEdge",            PT_INT,    0, 10000, SYNAPTICS_PROP_EDGES,	32,	3},
    {"FingerLow",             PT_INT,    0, 255,   SYNAPTICS_PROP_FINGER,	32,	0},
    {"FingerHigh",            PT_INT,    0, 255,   SYNAPTICS_PROP_FINGER,	32,	1},
    {"FingerPress",           PT_INT,    0, 256,   SYNAPTICS_PROP_FINGER,	32,	2},
    {"MaxTapTime",            PT_INT,    0, 1000,  SYNAPTICS_PROP_TAP_TIME,	32,	0},
    {"MaxTapMove",            PT_INT,    0, 2000,  SYNAPTICS_PROP_TAP_MOVE,	32,	0},
    {"MaxDoubleTapTime",      PT_INT,    0, 1000,  SYNAPTICS_PROP_TAP_DURATIONS,32,	1},
    {"SingleTapTimeout",      PT_INT,    0, 1000,  SYNAPTICS_PROP_TAP_DURATIONS,32,	0},
    {"ClickTime",             PT_INT,    0, 1000,  SYNAPTICS_PROP_TAP_DURATIONS,32,	2},
    {"FastTaps",              PT_BOOL,   0, 1,     SYNAPTICS_PROP_TAP_FAST,	8,	0},
    {"EmulateMidButtonTime",  PT_INT,    0, 1000,  SYNAPTICS_PROP_MIDDLE_TIMEOUT,32,	0},
    {"EmulateTwoFingerMinZ",  PT_INT,    0, 1000,  SYNAPTICS_PROP_TWOFINGER_PRESSURE,	32,	0},
    {"EmulateTwoFingerMinW",  PT_INT,    0, 15,    SYNAPTICS_PROP_TWOFINGER_WIDTH,	32,	0},
    {"VertScrollDelta",       PT_INT,    0, 1000,  SYNAPTICS_PROP_SCROLL_DISTANCE,	32,	0},
    {"HorizScrollDelta",      PT_INT,    0, 1000,  SYNAPTICS_PROP_SCROLL_DISTANCE,	32,	1},
    {"VertEdgeScroll",        PT_BOOL,   0, 1,     SYNAPTICS_PROP_SCROLL_EDGE,	8,	0},
    {"HorizEdgeScroll",       PT_BOOL,   0, 1,     SYNAPTICS_PROP_SCROLL_EDGE,	8,	1},
    {"CornerCoasting",        PT_BOOL,   0, 1,     SYNAPTICS_PROP_SCROLL_EDGE,	8,	2},
    {"VertTwoFingerScroll",   PT_BOOL,   0, 1,     SYNAPTICS_PROP_SCROLL_TWOFINGER,	8,	0},
    {"HorizTwoFingerScroll",  PT_BOOL,   0, 1,     SYNAPTICS_PROP_SCROLL_TWOFINGER,	8,	1},
    {"MinSpeed",              PT_DOUBLE, 0, 1.0,   SYNAPTICS_PROP_SPEED,	0, /*float */	0},
    {"MaxSpeed",              PT_DOUBLE, 0, 1.0,   SYNAPTICS_PROP_SPEED,	0, /*float */	1},
    {"AccelFactor",           PT_DOUBLE, 0, 1.0,   SYNAPTICS_PROP_SPEED,	0, /*float */	2},
    {"TrackstickSpeed",       PT_DOUBLE, 0, 200.0, SYNAPTICS_PROP_SPEED,	0, /*float */ 3},
    {"EdgeMotionMinZ",        PT_INT,    1, 255,   SYNAPTICS_PROP_EDGEMOTION_PRESSURE,  32,	0},
    {"EdgeMotionMaxZ",        PT_INT,    1, 255,   SYNAPTICS_PROP_EDGEMOTION_PRESSURE,  32,	1},
    {"EdgeMotionMinSpeed",    PT_INT,    0, 1000,  SYNAPTICS_PROP_EDGEMOTION_SPEED,     32,	0},
    {"EdgeMotionMaxSpeed",    PT_INT,    0, 1000,  SYNAPTICS_PROP_EDGEMOTION_SPEED,     32,	1},
    {"EdgeMotionUseAlways",   PT_BOOL,   0, 1,     SYNAPTICS_PROP_EDGEMOTION,   8,	0},
    {"UpDownScrolling",       PT_BOOL,   0, 1,     SYNAPTICS_PROP_BUTTONSCROLLING,  8,	0},
    {"LeftRightScrolling",    PT_BOOL,   0, 1,     SYNAPTICS_PROP_BUTTONSCROLLING,  8,	1},
    {"UpDownScrollRepeat",    PT_BOOL,   0, 1,     SYNAPTICS_PROP_BUTTONSCROLLING_REPEAT,   8,	0},
    {"LeftRightScrollRepeat", PT_BOOL,   0, 1,     SYNAPTICS_PROP_BUTTONSCROLLING_REPEAT,   8,	1},
    {"ScrollButtonRepeat",    PT_INT,    SBR_MIN , SBR_MAX, SYNAPTICS_PROP_BUTTONSCROLLING_TIME, 32,	0},
    {"TouchpadOff",           PT_INT,    0, 2,     SYNAPTICS_PROP_OFF,		8,	0},
    {"GuestMouseOff",         PT_BOOL,   0, 1,     SYNAPTICS_PROP_GUESTMOUSE,	8,	0},
    {"LockedDrags",           PT_BOOL,   0, 1,     SYNAPTICS_PROP_LOCKED_DRAGS,	8,	0},
    {"LockedDragTimeout",     PT_INT,    0, 30000, SYNAPTICS_PROP_LOCKED_DRAGS_TIMEOUT,	32,	0},
    {"RTCornerButton",        PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_TAP_ACTION,	8,	0},
    {"RBCornerButton",        PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_TAP_ACTION,	8,	1},
    {"LTCornerButton",        PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_TAP_ACTION,	8,	2},
    {"LBCornerButton",        PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_TAP_ACTION,	8,	3},
    {"TapButton1",            PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_TAP_ACTION,	8,	4},
    {"TapButton2",            PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_TAP_ACTION,	8,	5},
    {"TapButton3",            PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_TAP_ACTION,	8,	6},
    {"ClickFinger1",          PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_CLICK_ACTION,	8,	0},
    {"ClickFinger2",          PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_CLICK_ACTION,	8,	1},
    {"ClickFinger3",          PT_INT,    0, SYN_MAX_BUTTONS, SYNAPTICS_PROP_CLICK_ACTION,	8,	2},
    {"CircularScrolling",     PT_BOOL,   0, 1,     SYNAPTICS_PROP_CIRCULAR_SCROLLING,	8,	0},
    {"CircScrollDelta",       PT_DOUBLE, .01, 3,   SYNAPTICS_PROP_CIRCULAR_SCROLLING_DIST,	0 /* float */,	0},
    {"CircScrollTrigger",     PT_INT,    0, 8,     SYNAPTICS_PROP_CIRCULAR_SCROLLING_TRIGGER,	8,	0},
    {"CircularPad",           PT_BOOL,   0, 1,     SYNAPTICS_PROP_CIRCULAR_PAD,	8,	0},
    {"PalmDetect",            PT_BOOL,   0, 1,     SYNAPTICS_PROP_PALM_DETECT,	8,	0},
    {"PalmMinWidth",          PT_INT,    0, 15,    SYNAPTICS_PROP_PALM_DIMENSIONS,	32,	0},
    {"PalmMinZ",              PT_INT,    0, 255,   SYNAPTICS_PROP_PALM_DIMENSIONS,	32,	1},
    {"CoastingSpeed",         PT_DOUBLE, 0, 20,    SYNAPTICS_PROP_COASTING_SPEED,	0 /* float*/,	0},
    {"PressureMotionMinZ",    PT_INT,    1, 255,   SYNAPTICS_PROP_PRESSURE_MOTION,	32,	0},
    {"PressureMotionMaxZ",    PT_INT,    1, 255,   SYNAPTICS_PROP_PRESSURE_MOTION,	32,	1},
    {"PressureMotionMinFactor", PT_DOUBLE, 0, 10.0,SYNAPTICS_PROP_PRESSURE_MOTION_FACTOR,	0 /*float*/,	0},
    {"PressureMotionMaxFactor", PT_DOUBLE, 0, 10.0,SYNAPTICS_PROP_PRESSURE_MOTION_FACTOR,	0 /*float*/,	1},
    {"GrabEventDevice",       PT_BOOL,   0, 1,     SYNAPTICS_PROP_GRAB,	8,	0},
    {"TapAndDragGesture",     PT_BOOL,   0, 1,     SYNAPTICS_PROP_GESTURES,	8,	0},
    {"AreaLeftEdge",          PT_INT,    0, 10000, SYNAPTICS_PROP_AREA,	32,	0},
    {"AreaRightEdge",         PT_INT,    0, 10000, SYNAPTICS_PROP_AREA,	32,	1},
    {"AreaTopEdge",           PT_INT,    0, 10000, SYNAPTICS_PROP_AREA,	32,	2},
    {"AreaBottomEdge",        PT_INT,    0, 10000, SYNAPTICS_PROP_AREA,	32,	3},
    {"_CapLeftButton",        PT_BOOL,   0, 1,     SYNAPTICS_PROP_CAPABILITIES,	8,	0},
    {"_CapMiddleButton",      PT_BOOL,   0, 1,     SYNAPTICS_PROP_CAPABILITIES,	8,	1},
    {"_CapRightButton",       PT_BOOL,   0, 1,     SYNAPTICS_PROP_CAPABILITIES,	8,	2},
    {"_CapTwoFingers",        PT_BOOL,   0, 1,     SYNAPTICS_PROP_CAPABILITIES,	8,	3},
    {"_CapThreeFingers",      PT_BOOL,   0, 1,     SYNAPTICS_PROP_CAPABILITIES,	8,	4},
    { NULL, ParaType(0), 0, 0, 0, 0, 0           }
};

namespace Touchpad {

    int init_xinput_extension();
    int free_xinput_extension();

    const prop_list* get_properties_list();
    const void* get_parameter(const char* name);
    void set_parameter(const char* name, double variable);

    bool capability(const char* name);

    const char* get_device_name();
}

namespace Synaptics {
    typedef enum
    {
        NoTrigger=-1,
        AllCorners=0,
        TopEdge,
        TopRightCorner,
        RightEdge,
        BottomRightCorner,
        BottomEdge,
        BottomLeftCorner,
        LeftEdge,
        TopLeftCorner
    } ScrollTrigger;

    typedef enum
    {
        NoButton=0,
        Left,
        Middle,
        Right
    } Button;

    typedef enum
    {
        RightTop = 0,          // Right top corner
        RightBottom,           // Right bottom corner
        LeftTop,               // Left top corner
        LeftBottom,            // Left bottom corner
        OneFinger,             // Non-corner tap, one finger
        TwoFingers,            // Non-corner tap, two fingers
        ThreeFingers,          // Non-corner tap, three fingers
        MAX_TAP
    } TapType;
}

#endif	/* _TOUCHPAD_H */
