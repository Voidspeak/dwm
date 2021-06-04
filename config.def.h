/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = {
	"Iosevka:style=Regular:size=13",
	"Font Awesome 5 Free,Font Awesome 5 Free Solid:style=Solid:pixelsize=16",
	"Font Awesome 5 Free,Font Awesome 5 Free Regular:style=Regular:pixelsize=16",
	"Font Awesome 5 Brands,Font Awesome 5 Brands Regular:style=Regular:pixelsize=16"
};
static const char dmenufont[]       = "monospace:size=14";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_burgundy[]    = "#76092a";
static const char *colors[][3]      = {
	/*               fg         bg            border   */
	[SchemeNorm] = { col_gray3, col_gray1,    col_gray2 },
	[SchemeSel]  = { col_gray4, col_burgundy, col_burgundy },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class       instance    title           tags mask     isfloating   monitor */
	{ "Steam",     NULL,       NULL,           1 << 8,       1,           -1 },
	{ NULL,        NULL,       "Steam",        1 << 8,       1,           -1 },
	{ "copyq",     NULL,       NULL,           0,            1,           -1 },
	{ "spectacle", NULL,       NULL,           1 << 6,       1,           -1 },
	{ "Anydesk",   NULL,       NULL,           1 << 5,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define REFRESH_STATUS "killall -s USR1 slstatus"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]     = { "dmenu_run", "-l", "10", "-i", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_burgundy, "-sf", col_gray4, NULL };
static const char *termcmd[]      = { "alacritty", NULL };
static const char *lockcmd[]      = { "slock", NULL };
//static const char *sleepcmd[]     = { "slock", "sudo", "/usr/sbin/s2ram", NULL };
static const char *browsercmd[]   = { "firefox", NULL };
static const char *prtsccmd[]     = { "spectacle", NULL };
static const char *componcmd[]    = { "picom", "-b", NULL };
static const char *compoffcmd[]   = { "killall", "-s", "9", "picom", NULL };
static const char *dlaptopcmd[]   = { "display-select", "laptop", NULL };
static const char *dexterncmd[]   = { "display-select", "extern", NULL };
static const char *dmirrorcmd[]   = { "display-select", "mirror", NULL };
static const char *dextendcmd[]   = { "display-select", "extend", NULL };
static const char *backlupcmd[]   = { "backlight-control", "-i", "10", NULL };
static const char *backldowncmd[] = { "backlight-control", "-d", "10", NULL };
static const char *volupcmd[]     = { "/bin/sh", "-c", "volume-control -i 5; "REFRESH_STATUS, NULL };
static const char *voldowncmd[]   = { "/bin/sh", "-c", "volume-control -d 5; "REFRESH_STATUS, NULL };
static const char *voltogcmd[]    = { "/bin/sh", "-c", "volume-control -t; "REFRESH_STATUS, NULL };
static const char *mictogcmd[]    = { "/bin/sh", "-c", "mic-control -t; "REFRESH_STATUS, NULL };
static const char *camtogcmd[]    = { "/bin/sh", "-c", "cam-control -t; "REFRESH_STATUS, NULL };
static const char *kblayncmd[]    = { "/bin/sh", "-c", "keyboard-layout -n; "REFRESH_STATUS, NULL };
static const char *kblaypcmd[]    = { "/bin/sh", "-c", "keyboard-layout -p; "REFRESH_STATUS, NULL };
static const char *mpdstopcmd[]   = { "mpc", "stop", NULL };
static const char *mpdprevcmd[]   = { "mpc", "prev", NULL };
static const char *mpdnextcmd[]   = { "mpc", "next", NULL };
static const char *mpdtogcmd[]    = { "mpc", "toggle", NULL };
static const char *mpdclientcmd[] = { "alacritty", "-e", "ncmpcpp", NULL };
static const char *mpdvisualcmd[] = { "alacritty", "-o", "font.size=4", "-e", "cava", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_F12,    spawn,          {.v = lockcmd } },
//	{ MODKEY|ShiftMask,             XK_F12,    spawn,          {.v = sleepcmd } },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          {.v = browsercmd } },
	{ 0,                            XK_Print,  spawn,          {.v = prtsccmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_u,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_Delete, quit,           {0} },

	// Volume control
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = volupcmd } },
	{ 0,                            XF86XK_AudioLowerVolume, spawn, {.v = voldowncmd } },
	{ 0,                            XF86XK_AudioMute,        spawn, {.v = voltogcmd } },
	{ MODKEY,                       XK_F3,                   spawn, {.v = volupcmd } },
	{ MODKEY,                       XK_F2,                   spawn, {.v = voldowncmd } },
	{ MODKEY,                       XK_F1,                   spawn, {.v = voltogcmd } },

	// Microphone control
	{ 0,                            XF86XK_AudioMicMute,     spawn, {.v = mictogcmd } },
	{ MODKEY,                       XK_F4,                   spawn, {.v = mictogcmd } },

	// Camera control
	{ 0,                            XF86XK_WebCam,           spawn, {.v = camtogcmd } },
	{ MODKEY,                       XK_F10,                  spawn, {.v = camtogcmd } },

	// Display switching
	{ MODKEY|ShiftMask,             XK_F1,                   spawn, {.v = dlaptopcmd } },
	{ MODKEY|ShiftMask,             XK_F2,                   spawn, {.v = dexterncmd } },
	{ MODKEY|ShiftMask,             XK_F3,                   spawn, {.v = dmirrorcmd } },
	{ MODKEY|ShiftMask,             XK_F4,                   spawn, {.v = dextendcmd } },

	// Backlight control
	{ MODKEY|ControlMask,           XK_F3,                   spawn, {.v = backlupcmd } },
	{ MODKEY|ControlMask,           XK_F2,                   spawn, {.v = backldowncmd } },

	// Keyboard layout
	{ MODKEY,                       XK_Up,                   spawn, {.v = kblayncmd } },
	{ MODKEY,                       XK_Down,                 spawn, {.v = kblaypcmd } },

	// Compositor
	{ MODKEY|ShiftMask,             XK_p,                    spawn, {.v = componcmd } },
	{ MODKEY,                       XK_p,                    spawn, {.v = compoffcmd } },

	// Music Player Daemon control
	{ MODKEY,                       XK_F5,                   spawn, {.v = mpdstopcmd } },
	{ MODKEY,                       XK_F6,                   spawn, {.v = mpdprevcmd } },
	{ MODKEY,                       XK_F7,                   spawn, {.v = mpdtogcmd } },
	{ MODKEY,                       XK_F8,                   spawn, {.v = mpdnextcmd } },

	// Music Player Daemon client
	{ MODKEY|ShiftMask,             XK_F5,                   spawn, {.v = mpdclientcmd } },
	{ MODKEY|ShiftMask,             XK_F6,                   spawn, {.v = mpdvisualcmd } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

