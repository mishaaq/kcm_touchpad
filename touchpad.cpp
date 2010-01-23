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

#include <X11/Xdefs.h>
#include <X11/Xatom.h>
#include <X11/extensions/XI.h>
#include <X11/extensions/XInput.h>

#include <strings.h>
#include <string.h>
#include <math.h>
#include <map>

#include "touchpad.h"

int xi_opcode;

Display* display        = NULL;
XDevice* device         = NULL;
char* dev_name          = NULL;

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcasecmp(s1, s2) < 0;
  }
};
typedef std::map<const char*, struct Parameter*, ltstr> param_hash;

param_hash* parameters_map = NULL;
prop_list* properties_list = NULL;

static Display*
dp_init()
{
    XExtensionVersion *v	= NULL;
    Atom touchpad_type		= 0;
    Atom synaptics_property	= 0;
    int error			= 0;

    Display* dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Failed to connect to X Server.\n");
        error = 1;
        goto unwind;
    }

    v = XGetExtensionVersion(dpy, INAME);
    if (!v->present ||
        (v->major_version * 1000 + v->minor_version) < (XI_Add_DeviceProperties_Major * 1000
            + XI_Add_DeviceProperties_Minor)) {
        fprintf(stderr, "X server supports X Input %d.%d. I need %d.%d.\n",
                v->major_version, v->minor_version,
                XI_Add_DeviceProperties_Major,
                XI_Add_DeviceProperties_Minor);
        error = 1;
        goto unwind;
    }

    /* We know synaptics sets XI_TOUCHPAD for all the devices. */
    touchpad_type = XInternAtom(dpy, XI_TOUCHPAD, True);
    if (!touchpad_type) {
        fprintf(stderr, "XI_TOUCHPAD not initialised.\n");
        error = 1;
        goto unwind;
    }

    synaptics_property = XInternAtom(dpy, SYNAPTICS_PROP_EDGES, True);
    if (!synaptics_property) {
        fprintf(stderr, "Couldn't find synaptics properties. No synaptics "
                "driver loaded?\n");
        error = 1;
        goto unwind;
    }

unwind:
    XFree(v);
    if (error && dpy)
    {
        XCloseDisplay(dpy);
        dpy = NULL;
    }
    return dpy;
}

static XDevice *
dp_get_device(Display *dpy)
{
    XDevice* dev                = NULL;
    XDeviceInfo *info		= NULL;
    int ndevices		= 0;
    Atom touchpad_type		= 0;
    Atom synaptics_property	= 0;
    Atom *properties		= NULL;
    int nprops			= 0;
    int error			= 0;

    touchpad_type = XInternAtom(dpy, XI_TOUCHPAD, True);
    synaptics_property = XInternAtom(dpy, SYNAPTICS_PROP_EDGES, True);
    info = XListInputDevices(dpy, &ndevices);

    while(ndevices--) {
        if (info[ndevices].type == touchpad_type) {
            dev = XOpenDevice(dpy, info[ndevices].id);
            if (!dev) {
                fprintf(stderr, "Failed to open device '%s'.\n",
                        info[ndevices].name);
                error = 1;
                goto unwind;
            }

            properties = XListDeviceProperties(dpy, dev, &nprops);
            if (!properties || !nprops)
            {
                fprintf(stderr, "No properties on device '%s'.\n",
                        info[ndevices].name);
                error = 1;
                goto unwind;
            }

            while(nprops--)
            {
                if (properties[nprops] == synaptics_property)
                    break;
            }
            if (!nprops)
            {
                fprintf(stderr, "No synaptics properties on device '%s'.\n",
                        info[ndevices].name);
                error = 1;
                goto unwind;
            }

            dev_name = strdup(info[ndevices].name);
            printf("Recognized device: %s\n", dev_name);

            break; /* Yay, device is suitable */
        }
    }

unwind:
    XFree(properties);
    XFreeDeviceList(info);
    if (!dev)
        fprintf(stderr, "Unable to find a synaptics device.\n");
    else if (error && dev)
    {
        XCloseDevice(dpy, dev);
        dev = NULL;
    }
    return dev;
}

static void*
dp_get_parameter(Display *dpy, XDevice *dev, const char *name) {
    Atom a, type, float_type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char* data;
    int len;

    union flong *f;
    long *i;
    char *b;

    float_type = XInternAtom(dpy, XATOM_FLOAT, True);
    if (!float_type)
        fprintf(stderr, "Float properties not available.\n");

    struct Parameter *par = (*parameters_map)[name];
    a = XInternAtom(dpy, par->prop_name, True);
    if (!a) {
        fprintf(stderr, "    %-23s = missing\n", par->name);
        return NULL;
    }

    len = 1 + ((par->prop_offset * (par->prop_format ? par->prop_format : 32)/8))/4;

    XGetDeviceProperty(dpy, dev, a, 0, len, False,
                            AnyPropertyType, &type, &format,
                            &nitems, &bytes_after, &data);

    switch(par->prop_format) {
        case 8:
        {
            if (format != par->prop_format || type != XA_INTEGER) {
                fprintf(stderr, "   %-23s = format mismatch (%d)\n",
                        par->name, format);
                break;
            }

            b = (char*)data;
            char* b_value = new char;
            *b_value = b[par->prop_offset];
            return b_value;
            break;
        }
        case 32:
        {
            if (format != par->prop_format || type != XA_INTEGER) {
                fprintf(stderr, "   %-23s = format mismatch (%d)\n",
                        par->name, format);
                break;
            }

            i = (long*)data;
            int* i_value = new int;
            *i_value = i[par->prop_offset];
            return i_value;
            break;
        }
        case 0: /* Float */
        {
            if (format != 32 || type != float_type) {
                fprintf(stderr, "   %-23s = format mismatch (%d)\n",
                        par->name, format);
                break;
            }

            f = (union flong*)data;
            double* d_value = new double;
            *d_value = f[par->prop_offset].f;
            return d_value;
            break;
        }
    }

    XFree(data);
    return 0;
}

static param_hash*
dp_prepare_parameters_hash(Display *dpy) {
    param_hash* parameters_hash = new param_hash;

    int j;
    for (j = 0; params[j].name; j++) {
        if (XInternAtom(dpy, params[j].prop_name, True))
            (*parameters_hash)[params[j].name] = &params[j];
    }

    return parameters_hash;
}

static prop_list*
dp_prepare_properties_list(Display *dpy) {
    prop_list* properties_list = new prop_list;

    int j;
    for (j = 0; params[j].prop_name; j++) {
        if (XInternAtom(dpy, params[j].prop_name, True))
            properties_list->push_back(params[j].prop_name);
        else
            fprintf(stderr, "Property for '%s' not available. Skipping.\n", params[j].prop_name);
    }

    return properties_list;
}

static void
dp_set_parameter(Display *dpy, XDevice* dev, const char* name, double var)
{
    Atom prop, type, float_type;
    int format;
    unsigned char* data;
    unsigned long nitems, bytes_after;

    union flong *f;
    long *n;
    char *b;

    float_type = XInternAtom(dpy, XATOM_FLOAT, True);
    if (!float_type)
        fprintf(stderr, "Float properties not available.\n");

    struct Parameter *par = (*parameters_map)[name];
    prop = XInternAtom(dpy, par->prop_name, True);
    if (!prop) {
        fprintf(stderr, "Property for '%s' not available. Skipping.\n", par->name);
    }

    XGetDeviceProperty(dpy, dev, prop, 0, 1000, False, AnyPropertyType,
                            &type, &format, &nitems, &bytes_after, &data);

    switch(par->prop_format)
    {
        case 8:
            if (format != par->prop_format || type != XA_INTEGER) {
                fprintf(stderr, "   %-23s = format mismatch (%d)\n",
                        par->name, format);
                break;
            }
            b = (char*)data;
            b[par->prop_offset] = rint(var);
            break;
        case 32:
            if (format != par->prop_format || type != XA_INTEGER) {
                fprintf(stderr, "   %-23s = format mismatch (%d)\n",
                        par->name, format);
                break;
            }
            n = (long*)data;
            n[par->prop_offset] = rint(var);
            break;
        case 0: /* float */
            if (format != 32 || type != float_type) {
                fprintf(stderr, "   %-23s = format mismatch (%d)\n",
                        par->name, format);
                break;
            }
            f = (union flong*)data;
            f[par->prop_offset].f = var;
            break;
    }

    XChangeDeviceProperty(dpy, dev, prop, type, format,
                            PropModeReplace, data, nitems);
    XFlush(dpy);
}


int
Touchpad::init_xinput_extension() {
    display = dp_init();
    if (display == NULL)
        return GET_DISPLAY_FAILED;

    device = dp_get_device(display);
    if (device == NULL)
        return GET_DEVICE_FAILED;

    parameters_map = dp_prepare_parameters_hash(display);
    properties_list = dp_prepare_properties_list(display);

    return 0;
}

const prop_list*
Touchpad::get_properties_list() {
    return properties_list;
}

const void*
Touchpad::get_parameter(const char* name) {
    if (display && device)
        return dp_get_parameter(display, device, name);
    return NULL;
}

void
Touchpad::set_parameter(const char* name, double variable) {
    if (display && device && variable != -1)
        dp_set_parameter(display, device, name, variable);
}

bool
Touchpad::capability(const char* name) {
    if (display && device) {
	char *cap;

	cap = (char *)dp_get_parameter(display, device, name);
	if (cap)
	    return *cap == 1;
    }

    return true;
}

const char*
Touchpad::get_device_name() {
    return dev_name;
}


int
Touchpad::free_xinput_extension() {
    if (display && device) {
        XCloseDevice(display, device);
        XSync(display, True);
        XCloseDisplay(display);
    }

    free(parameters_map);
    free(properties_list);
    free(dev_name);

    return 0;
}
