#pragma once

#define _GRAY       "\x1b[90m"
#define _RED        "\x1b[91m"
#define _GREEN      "\x1b[92m"
#define _YELLOW     "\x1b[93m"
#define _BLUE       "\x1b[94m"
#define _MAGENTA    "\x1b[95m"
#define _CYAN       "\x1b[96m"
#define _CYAN2      "\x1b[36m"

#define _BG_RED     "\x1b[41m"
#define _BG_GREEN   "\x1b[42m"
#define _BG_BLUE    "\x1b[44m"
#define _BG_YELL    "\x1b[43m"
#define _BG_MAG     "\x1b[45m"
#define _BG_CYAN    "\x1b[46m"

#define _BOLD       "\x1b[1m"
#define _PALE       "\x1b[2m"
#define _INTALIC    "\x1b[3m"
#define _UNDER      "\x1b[4m"
#define _FLASH      "\x1b[5m"

#define _RST        "\x1b[0m"

int PRINT_ON = 1; // add ability to turn off printf globally

#define pf(fmt, ...)            if(PRINT_ON)printf(fmt, ##__VA_ARGS__)
#define pf_gray(fmt, ...)       pf(_GRAY fmt _RST, ##__VA_ARGS__)
#define pf_red(fmt, ...)        pf(_RED fmt _RST, ##__VA_ARGS__)
#define pf_green(fmt, ...)      pf(_GREEN fmt _RST, ##__VA_ARGS__)
#define pf_blue(fmt, ...)       pf(_BLUE fmt _RST, ##__VA_ARGS__)
#define pf_magenta(fmt, ...)    pf(_MAGENTA fmt _RST, ##__VA_ARGS__)
#define pf_yellow(fmt, ...)     pf(_YELLOW fmt _RST, ##__VA_ARGS__)
#define pf_bold(fmt, ...)       pf(_BOLD fmt _RST, ##__VA_ARGS__)
