#ifndef SW_RESOURCES
#define SW_RESOURCES
#define WATCH_HEIGHT 240
#define WATCH_WIDTH 240
extern const unsigned char menu_alarm[];
extern const unsigned char menu_events[];
extern const unsigned char menu_media[];
extern const unsigned char menu_pedometer[];
extern const unsigned char menu_stopwatch[];
extern const unsigned char* menu_frames[];
extern const unsigned char watch[];

extern const unsigned char font60[];
extern const unsigned char font40[];

enum SW_MENU_T {
    SW_MENU_ALARM,
    SW_MENU_CHRONO,
    SW_MENU_EVENT,
    SW_MENU_MEDIA,
    SW_MENU_STEP,
    SW_MENU_SIZE
};

#define MENU_S(CURRENT)                                                        \
    CURRENT == SW_MENU_ALARM    ? "MENU_ALARM"                                 \
    : CURRENT == SW_MENU_CHRONO ? "MENU_CHRONO"                                \
    : CURRENT == SW_MENU_EVENT  ? "MENU_EVENT"                                 \
    : CURRENT == SW_MENU_MEDIA  ? "MENU_MEDIA"                                 \
    : CURRENT == SW_MENU_STEP   ? "MENU_STEP"                                  \
                                : "NONE"
#endif
