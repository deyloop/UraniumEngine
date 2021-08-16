#include <X11/Xos.h>
#include <X11/Xutil.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/utsname.h>

int main(int argc, char **argv) {

  Display *dpy = XOpenDisplay(NULL);
  if (dpy == nullptr) {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }

  int s = DefaultScreen(dpy);
  Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, s), 10, 10, 660, 200, 1,
                                   BlackPixel(dpy, s), WhitePixel(dpy, s));

  XSelectInput(dpy, win, ExposureMask | KeyPressMask);
  XMapWindow(dpy, win);

  XStoreName(dpy, win, "Uranium");

  Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);

  bool uname_ok = false;
  struct utsname sname;
  int ret = uname(&sname);
  if (ret != -1) {
    uname_ok = true;
  }

  XEvent e;
  while (true) {
    XNextEvent(dpy, &e);
    if (e.type == Expose) {
      int y_offset = 20;

      const char *s1 = "X11 test app under Linux";
      const char *s2 = "(C)2021 AnurupDey";

      XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, s1, strlen(s1));
      y_offset += 20;
      XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, s2, strlen(s2));
      y_offset += 20;

      if (uname_ok) {
        char buf[256] = {0};

        sprintf(buf, "System information:");
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf,
                    strlen(buf));
        y_offset += 15;

        sprintf(buf, "- System: %s", sname.sysname);
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf,
                    strlen(buf));
        y_offset += 15;

        sprintf(buf, "- Release: %s", sname.release);
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf,
                    strlen(buf));
        y_offset += 15;

        sprintf(buf, "- Version: %s", sname.version);
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf,
                    strlen(buf));
        y_offset += 15;

        sprintf(buf, "- Machine: %s", sname.machine);
        XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf,
                    strlen(buf));
        y_offset += 20;
      }

      XWindowAttributes wa;
      XGetWindowAttributes(dpy, win, &wa);
      int width = wa.width;
      int height = wa.height;
      char buf[128] = {0};
      sprintf(buf, "Current window size: %dx%d", width, height);
      XDrawString(dpy, win, DefaultGC(dpy, s), 10, y_offset, buf, strlen(buf));
      y_offset += 20;
    }

    if (e.type == KeyPress) {
      char buf[128] = {0};
      KeySym keysym;
      int len = XLookupString(&e.xkey, buf, sizeof buf, &keysym, NULL);
      if (keysym == XK_Escape)
        break;
    }

    if ((e.type == ClientMessage) &&
        (static_cast<unsigned int>(e.xclient.data.l[0]) == WM_DELETE_WINDOW)) {
      break;
    }
  }

  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);

  return 0;
}