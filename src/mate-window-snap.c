#define _POSIX_C_SOURCE 200809L

#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>

#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MATE_WINDOW_SNAP_VERSION "1.0.13"
#define DEBUG_LOG_PATH "/tmp/mate-window-snap.log"

enum { MAX_MONITORS = 16 };
enum { GEOMETRY_TOLERANCE = 64 };

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

typedef struct {
    int left;
    int right;
    int top;
    int bottom;
} FrameExtents;

typedef enum {
    SNAP_TOP_LEFT,
    SNAP_LEFT,
    SNAP_BOTTOM_LEFT,
    SNAP_MAXIMIZE,
    SNAP_TOP_RIGHT,
    SNAP_RIGHT,
    SNAP_BOTTOM_RIGHT,
} SnapAction;

typedef struct {
    KeySym symbol;
    SnapAction action;
} KeyBinding;

static const KeyBinding bindings[] = {
    { XK_KP_7, SNAP_TOP_LEFT },
    { XK_KP_4, SNAP_LEFT },
    { XK_KP_1, SNAP_BOTTOM_LEFT },
    { XK_KP_5, SNAP_MAXIMIZE },
    { XK_KP_9, SNAP_TOP_RIGHT },
    { XK_KP_6, SNAP_RIGHT },
    { XK_KP_3, SNAP_BOTTOM_RIGHT },
};

static Display *display;
static Window root;
static unsigned int num_lock_mask;
static Atom net_active_window;
static Atom net_moveresize_window;
static Atom net_wm_state;
static Atom net_wm_state_maximized_horz;
static Atom net_wm_state_maximized_vert;

static bool debug_enabled(void)
{
    const char *value = getenv("MATE_WINDOW_SNAP_DEBUG");
    return value != NULL && value[0] != '\0' && strcmp(value, "0") != 0;
}

static void debug_print(const char *format, ...)
{
    static FILE *log;
    va_list args;

    if (log == NULL) {
        /* O_NOFOLLOW stops a pre-planted symlink in /tmp from redirecting the log. */
        int fd = open(DEBUG_LOG_PATH, O_WRONLY | O_CREAT | O_APPEND | O_NOFOLLOW, 0600);
        if (fd >= 0) {
            log = fdopen(fd, "a");
            if (log == NULL) {
                close(fd);
            } else {
                setvbuf(log, NULL, _IOLBF, 0);
            }
        }
    }

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    if (log != NULL) {
        va_start(args, format);
        vfprintf(log, format, args);
        va_end(args);
    }
}

static int compare_monitors(const void *left, const void *right)
{
    const Rectangle *left_monitor = left;
    const Rectangle *right_monitor = right;
    return (left_monitor->x > right_monitor->x) - (left_monitor->x < right_monitor->x);
}

static bool ranges_overlap(int first_start, int first_end, int second_start, int second_end)
{
    return first_start <= second_end && second_start <= first_end;
}

/* Marco rejects automated moves that overlap a panel, so targets must stay inside this area. */
static Rectangle usable_area(Rectangle monitor)
{
    Rectangle area = monitor;
    Atom type;
    int format;
    unsigned long count;
    unsigned long bytes_after;
    unsigned char *list = NULL;
    int screen_width = DisplayWidth(display, DefaultScreen(display));
    int screen_height = DisplayHeight(display, DefaultScreen(display));
    int monitor_right = monitor.x + monitor.width - 1;
    int monitor_bottom = monitor.y + monitor.height - 1;
    Atom strut_property = XInternAtom(display, "_NET_WM_STRUT_PARTIAL", False);

    if (XGetWindowProperty(display, root, XInternAtom(display, "_NET_CLIENT_LIST", False), 0, 4096,
                           False, XA_WINDOW, &type, &format, &count, &bytes_after,
                           &list) != Success ||
        list == NULL || format != 32) {
        XFree(list);
        return area;
    }

    for (unsigned long index = 0; index < count; index++) {
        unsigned long item_count;
        unsigned char *data = NULL;

        if (XGetWindowProperty(display, ((Window *)list)[index], strut_property, 0, 12, False,
                               XA_CARDINAL, &type, &format, &item_count, &bytes_after,
                               &data) != Success ||
            data == NULL || format != 32 || item_count != 12) {
            XFree(data);
            continue;
        }

        unsigned long *strut = (unsigned long *)data;
        if (strut[0] > 0 && (int)strut[0] > area.x &&
            ranges_overlap((int)strut[4], (int)strut[5], monitor.y, monitor_bottom)) {
            area.width -= (int)strut[0] - area.x;
            area.x = (int)strut[0];
        }
        if (strut[1] > 0 && screen_width - (int)strut[1] < area.x + area.width &&
            ranges_overlap((int)strut[6], (int)strut[7], monitor.y, monitor_bottom)) {
            area.width = screen_width - (int)strut[1] - area.x;
        }
        if (strut[2] > 0 && (int)strut[2] > area.y &&
            ranges_overlap((int)strut[8], (int)strut[9], monitor.x, monitor_right)) {
            area.height -= (int)strut[2] - area.y;
            area.y = (int)strut[2];
        }
        if (strut[3] > 0 && screen_height - (int)strut[3] < area.y + area.height &&
            ranges_overlap((int)strut[10], (int)strut[11], monitor.x, monitor_right)) {
            area.height = screen_height - (int)strut[3] - area.y;
        }
        XFree(data);
    }
    XFree(list);
    return area;
}

static FrameExtents frame_extents(Window window)
{
    FrameExtents extents = { 0 };
    Atom property = XInternAtom(display, "_GTK_FRAME_EXTENTS", True);
    Atom type;
    int format;
    unsigned long item_count;
    unsigned long bytes_after;
    unsigned char *data = NULL;

    if (property != None &&
        XGetWindowProperty(display, window, property, 0, 4, False, XA_CARDINAL,
                           &type, &format, &item_count, &bytes_after, &data) == Success &&
        type == XA_CARDINAL && format == 32 && item_count == 4 && data != NULL) {
        unsigned long *values = (unsigned long *)data;
        extents.left = (int)values[0];
        extents.right = (int)values[1];
        extents.top = (int)values[2];
        extents.bottom = (int)values[3];
    }
    XFree(data);
    return extents;
}

static Rectangle expanded_rectangle(Rectangle target, FrameExtents extents)
{
    target.x -= extents.left;
    target.y -= extents.top;
    target.width += extents.left + extents.right;
    target.height += extents.top + extents.bottom;
    return target;
}

static size_t monitors(Rectangle result[MAX_MONITORS])
{
    XRRScreenResources *resources = XRRGetScreenResourcesCurrent(display, root);
    size_t count = 0;

    if (resources != NULL) {
        for (int output_index = 0; output_index < resources->noutput && count < MAX_MONITORS;
             output_index++) {
            XRROutputInfo *output = XRRGetOutputInfo(display, resources,
                                                      resources->outputs[output_index]);
            if (output == NULL || output->connection != RR_Connected || output->crtc == None) {
                XRRFreeOutputInfo(output);
                continue;
            }

            XRRCrtcInfo *crtc = XRRGetCrtcInfo(display, resources, output->crtc);
            XRRFreeOutputInfo(output);
            if (crtc == NULL || crtc->width == 0 || crtc->height == 0) {
                XRRFreeCrtcInfo(crtc);
                continue;
            }

            bool duplicate = false;
            for (size_t monitor_index = 0; monitor_index < count; monitor_index++) {
                duplicate = result[monitor_index].x == crtc->x &&
                            result[monitor_index].y == crtc->y &&
                            result[monitor_index].width == (int)crtc->width &&
                            result[monitor_index].height == (int)crtc->height;
            }
            if (!duplicate) {
                result[count++] = (Rectangle){ crtc->x, crtc->y, (int)crtc->width,
                                                (int)crtc->height };
            }
            XRRFreeCrtcInfo(crtc);
        }
        XRRFreeScreenResources(resources);
    }

    if (count == 0) {
        result[count++] = (Rectangle){ 0, 0, DisplayWidth(display, DefaultScreen(display)),
                                       DisplayHeight(display, DefaultScreen(display)) };
    }
    qsort(result, count, sizeof(*result), compare_monitors);
    return count;
}

static Window focused_window(void)
{
    Atom type;
    int format;
    unsigned long item_count;
    unsigned long bytes_after;
    unsigned char *data = NULL;
    Window window = None;

    if (XGetWindowProperty(display, root, net_active_window, 0, 1, False, XA_WINDOW,
                           &type, &format, &item_count, &bytes_after, &data) == Success &&
        data != NULL && format == 32 && item_count == 1) {
        window = *(Window *)data;
    }
    XFree(data);
    return window;
}

static size_t monitor_for_window(Window window, const Rectangle monitor_list[], size_t count)
{
    XWindowAttributes attributes;
    Window child;
    int root_x;
    int root_y;

    if (!XGetWindowAttributes(display, window, &attributes) ||
        !XTranslateCoordinates(display, window, root, 0, 0, &root_x, &root_y, &child)) {
        return 0;
    }

    int center_x = root_x + attributes.width / 2;
    int center_y = root_y + attributes.height / 2;
    for (size_t index = 0; index < count; index++) {
        if (center_x >= monitor_list[index].x &&
            center_x < monitor_list[index].x + monitor_list[index].width &&
            center_y >= monitor_list[index].y &&
            center_y < monitor_list[index].y + monitor_list[index].height) {
            return index;
        }
    }
    return 0;
}

static bool matches(Window window, Rectangle rectangle)
{
    XWindowAttributes attributes;
    Window child;
    int x;
    int y;

    if (!XGetWindowAttributes(display, window, &attributes) ||
        !XTranslateCoordinates(display, window, root, 0, 0, &x, &y, &child)) {
        return false;
    }

    return abs(x - rectangle.x) <= GEOMETRY_TOLERANCE &&
           abs(y - rectangle.y) <= GEOMETRY_TOLERANCE &&
           abs(x + attributes.width - rectangle.x - rectangle.width) <= GEOMETRY_TOLERANCE &&
           abs(y + attributes.height - rectangle.y - rectangle.height) <= GEOMETRY_TOLERANCE;
}

static Rectangle snapped_rectangle(Rectangle area, SnapAction action)
{
    int half_width = area.width / 2;
    int half_height = area.height / 2;
    Rectangle result = area;

    switch (action) {
    case SNAP_TOP_LEFT:
        result.width = half_width;
        result.height = half_height;
        break;
    case SNAP_LEFT:
        result.width = half_width;
        break;
    case SNAP_BOTTOM_LEFT:
        result.width = half_width;
        result.height = half_height;
        result.y += half_height;
        break;
    case SNAP_TOP_RIGHT:
        result.width = half_width;
        result.height = half_height;
        result.x += half_width;
        break;
    case SNAP_RIGHT:
        result.width = half_width;
        result.x += half_width;
        break;
    case SNAP_BOTTOM_RIGHT:
        result.width = half_width;
        result.height = half_height;
        result.x += half_width;
        result.y += half_height;
        break;
    case SNAP_MAXIMIZE:
        break;
    }
    return result;
}

static void move_resize_window(Window window, Rectangle target)
{
    XEvent state_event = { .xclient = {
        .type = ClientMessage,
        .window = window,
        .message_type = net_wm_state,
        .format = 32,
        .data.l = {
            0,
            net_wm_state_maximized_horz,
            net_wm_state_maximized_vert,
            2,
            0,
        },
    } };
    XEvent event = { .xclient = {
        .type = ClientMessage,
        .window = window,
        .message_type = net_moveresize_window,
        .format = 32,
        .data.l = {
            StaticGravity | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11),
            target.x,
            target.y,
            target.width,
            target.height,
        },
    } };

    XSendEvent(display, root, False, SubstructureRedirectMask | SubstructureNotifyMask, &state_event);
    XSendEvent(display, root, False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
}

static void snap_focused_window(SnapAction action)
{
    Window window = focused_window();
    Rectangle monitor_list[MAX_MONITORS];
    size_t monitor_count;
    size_t monitor_index;
    Rectangle area;
    Rectangle target;
    Rectangle requested;
    FrameExtents extents;
    XWindowAttributes attributes;
    bool is_current_target;
    bool crossing_monitor = false;

    if (window == None) {
        return;
    }
    monitor_count = monitors(monitor_list);
    monitor_index = monitor_for_window(window, monitor_list, monitor_count);
    area = usable_area(monitor_list[monitor_index]);
    extents = frame_extents(window);
    target = snapped_rectangle(area, action);
    is_current_target = matches(window, expanded_rectangle(target, extents));

    if (action == SNAP_LEFT && monitor_index > 0 && is_current_target) {
        crossing_monitor = true;
        area = usable_area(monitor_list[monitor_index - 1]);
        if (XGetWindowAttributes(display, window, &attributes)) {
            target.width = attributes.width - extents.left - extents.right;
            target.height = attributes.height - extents.top - extents.bottom;
            if (target.height > area.height) {
                target.height = area.height;
            }
            target.x = area.x + area.width - target.width;
            target.y = area.y;
        }
    } else if (action == SNAP_RIGHT && monitor_index + 1 < monitor_count && is_current_target) {
        crossing_monitor = true;
        area = usable_area(monitor_list[monitor_index + 1]);
        if (XGetWindowAttributes(display, window, &attributes)) {
            target.width = attributes.width - extents.left - extents.right;
            target.height = attributes.height - extents.top - extents.bottom;
            if (target.height > area.height) {
                target.height = area.height;
            }
            target.x = area.x;
            target.y = area.y;
        }
    }

    requested = expanded_rectangle(target, extents);
    if (debug_enabled()) {
        debug_print("snap action=%d window=0x%lx monitor=%zu/%zu current_target=%d cross=%d "
                    "target=%d,%d %dx%d request=%d,%d %dx%d extents=%d,%d,%d,%d\n",
                    action, window, monitor_index, monitor_count, is_current_target,
                    crossing_monitor, target.x, target.y, target.width, target.height,
                    requested.x, requested.y, requested.width, requested.height,
                    extents.left, extents.right, extents.top, extents.bottom);
    }
    move_resize_window(window, requested);
}

static void find_num_lock_mask(void)
{
    XModifierKeymap *modifier_map = XGetModifierMapping(display);
    if (modifier_map == NULL) {
        return;
    }

    for (int modifier = 0; modifier < 8; modifier++) {
        for (int key = 0; key < modifier_map->max_keypermod; key++) {
            KeyCode keycode = modifier_map->modifiermap[modifier * modifier_map->max_keypermod + key];
            if (keycode != 0 && XkbKeycodeToKeysym(display, keycode, 0, 0) == XK_Num_Lock) {
                num_lock_mask = 1U << modifier;
            }
        }
    }
    XFreeModifiermap(modifier_map);
}

static void grab_keys(void)
{
    const unsigned int ignored_caps_lock[] = { 0, LockMask };
    for (size_t binding = 0; binding < sizeof(bindings) / sizeof(bindings[0]); binding++) {
        KeyCode keycode = XKeysymToKeycode(display, bindings[binding].symbol);
        for (size_t caps_lock = 0; caps_lock < sizeof(ignored_caps_lock) / sizeof(ignored_caps_lock[0]);
             caps_lock++) {
            XGrabKey(display, (int)keycode, Mod4Mask | num_lock_mask | ignored_caps_lock[caps_lock], root,
                     True, GrabModeAsync, GrabModeAsync);
        }
    }
}

static const KeyBinding *binding_for_keycode(KeyCode keycode)
{
    for (size_t binding = 0; binding < sizeof(bindings) / sizeof(bindings[0]); binding++) {
        if (XKeysymToKeycode(display, bindings[binding].symbol) == keycode) {
            return &bindings[binding];
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("mate-window-snap %s\n", MATE_WINDOW_SNAP_VERSION);
        return EXIT_SUCCESS;
    }
    if (argc != 1) {
        fprintf(stderr, "Usage: %s [--version]\n", argv[0]);
        return EXIT_FAILURE;
    }

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fputs("mate-window-snap: cannot connect to the X display\n", stderr);
        return EXIT_FAILURE;
    }

    root = RootWindow(display, DefaultScreen(display));
    net_active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    net_moveresize_window = XInternAtom(display, "_NET_MOVERESIZE_WINDOW", False);
    net_wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    net_wm_state_maximized_horz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    net_wm_state_maximized_vert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
    find_num_lock_mask();
    grab_keys();
    XSync(display, False);

    for (;;) {
        XEvent event;
        XNextEvent(display, &event);
        switch (event.type) {
        case KeyPress: {
            const KeyBinding *binding = binding_for_keycode(event.xkey.keycode);
            bool accepted = binding != NULL &&
                            (event.xkey.state & num_lock_mask) == num_lock_mask;
            if (debug_enabled()) {
                debug_print("key keycode=%u state=0x%x action=%d accepted=%d\n",
                            event.xkey.keycode, event.xkey.state,
                            binding != NULL ? (int)binding->action : -1, accepted);
            }
            if (accepted) {
                snap_focused_window(binding->action);
            }
            break;
        }
        }
    }
}