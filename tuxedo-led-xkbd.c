#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xlibint.h>
#include <err.h>


int main(int argc, char *argv[]) {
    Display *dpy;
    XkbEvent ev;
    char *mainColor="0xff4400", *alterColor="0x44ff00";

    if(argc == 3){
        mainColor = argv[1];
        alterColor = argv[2];
    }

    printf("Config: main %s alter %s\n", mainColor, alterColor);

    int xkbEventType, xkbError, reason_rtrn, mjr, mnr;
    char *display_name = NULL;
    
    int group_index, old_group_index = -1;
    
    mjr = XkbMajorVersion;
    mnr = XkbMinorVersion;

    dpy = XkbOpenDisplay(display_name, &xkbEventType, &xkbError, &mjr, &mnr, &reason_rtrn);

    if (dpy == NULL) {
        warnx("Can't open display named %s", XDisplayName(display_name));
        switch (reason_rtrn) {
            case XkbOD_BadLibraryVersion :
            case XkbOD_BadServerVersion :
                warnx("xxkb was compiled with XKB version %d.%02d", XkbMajorVersion, XkbMinorVersion);
                warnx("But %s uses incompatible version %d.%02d",
                      reason_rtrn == XkbOD_BadLibraryVersion ? "Xlib" : "Xserver",
                      mjr, mnr);
                break;

            case XkbOD_ConnectionRefused :
                warnx("Connection refused");
                break;

            case XkbOD_NonXkbServer:
                warnx("XKB extension not present");
                break;

            default:
                warnx("Unknown error %d from XkbOpenDisplay", reason_rtrn);
                break;
        }
        exit(1);
    }

    XkbSelectEventDetails(dpy, XkbUseCoreKbd, XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);
    while (1) {
        XNextEvent(dpy, &ev.core);
        if (ev.any.xkb_type == XkbStateNotify) {
            group_index = ev.state.locked_group;
            if(old_group_index != group_index) {
                old_group_index = group_index;
                FILE *f = fopen("/sys/devices/platform/tuxedo_keyboard/color_left", "wt");
                if(group_index == 0){
                    fputs(mainColor, f);
                } else {
                    fputs(alterColor, f);
                }
                fclose(f);
            }

        }
    }
}
