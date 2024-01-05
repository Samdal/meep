#include <libnotify/notify.h>

#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char * argv[]) {
 int xiOpcode, queryEvent, queryError;

 notify_init("mehehe meep");

 Display * disp = XOpenDisplay(":0");
 Window root = DefaultRootWindow(disp);
 if (!XQueryExtension(disp, "XInputExtension", &xiOpcode, &queryEvent, &queryError)) return 2;

 {
  XIEventMask m;
  m.deviceid = XIAllMasterDevices;
  m.mask_len = XIMaskLen(XI_LASTEVENT);
  m.mask = calloc(m.mask_len, sizeof(char));
  XISetMask(m.mask, XI_RawKeyPress);
  XISelectEvents(disp, root, &m, 1);
  XSync(disp, false);
  free(m.mask);
 }

 XkbSelectEventDetails(disp, XkbUseCoreKbd, XkbStateNotify, XkbGroupStateMask, XkbGroupStateMask);

 char* matches[] = {"mehehe", "meep", "goop"};
 int states[sizeof(matches) / sizeof(*matches)] = {0};

 for (;;) {
  XEvent event;
  XGenericEventCookie *cookie = (XGenericEventCookie*)&event.xcookie;
  XNextEvent(disp, &event);

  if (XGetEventData(disp, cookie)) {
   if (cookie->extension == xiOpcode && cookie->evtype == XI_RawKeyPress) {
    XIRawEvent *ev = cookie->data;
    KeySym s = XkbKeycodeToKeysym(disp, ev->detail, 0, 0);
    char *str = XKeysymToString(s);
    if (NULL == str) continue;

    for (int i = 0; i < sizeof(matches) / sizeof(*matches); i++) {
     if (*str == matches[i][states[i]]) {
      states[i]++;
      if (matches[i][states[i]] == 0) {
       NotifyNotification* notif = notify_notification_new(matches[i], matches[i], NULL);
       notify_notification_show(notif, NULL);
       states[i] = 0;
      }
     } else {
      states[i] = 0;
     }
    }
   }
   XFreeEventData(disp, cookie);
  }
 }
}
