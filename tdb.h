#ifndef __TDB_HEADER__
#define __TDB_HEADER__

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TDB_PIXEL "██"
#define TDB_PIXEL1 "##"
#define TDB_PIXEL2 "●●"
#define TDB_PIXEL3 "◼◼"
#define TDB_PIXEL4 "@@"
#define TDB_PIXEL5 "//"

#ifdef __unix__
#include <termios.h>
#include <sys/ioctl.h>
static struct termios _default; 
#endif

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
static DWORD _default;
static HANDLE _input;
static HANDLE _output;
#endif

enum TDB_Keys {
  TDB_SPACE = 32,
  TDB_BACKSPACE = 127,
  TDB_UP = 1000,
  TDB_DOWN,
  TDB_LEFT,
  TDB_RIGHT,
  TDB_HOME,
  TDB_END
};

typedef struct TDB_Pos { 
  int col;
  int row;
} TDB_Pos;

typedef struct TDB_BitMap {
  unsigned short width;
  unsigned short height;
  unsigned short row_size;
  unsigned short bytes_per_pixel;
  uint8_t * data;
} TDB_BitMap;

static void beforeExit();
void TDB_Setup(int blocking);

void TDB_Write(const char * t, int x, int y);
void TDB_WriteF(int x, int y, const char * t, ...);
void TDB_Clear();

void TDB_SetCursor(int col, int row);

void TDB_ShowCursor();
void TDB_HideCursor();
TDB_Pos TDB_GetCursor();
void TDB_SetCursor(int col, int row);

TDB_Pos TDB_GetSize();
int TDB_GetKey();

void TDB_DrawLine(const char * c, int x1, int y1, int x2, int y2);
void TDB_DrawRect(const char * c, int x, int y, int w, int h);
void TDB_DrawTria(const char * c, int x, int y, int x1, int y1, int x2, int y2);
void TDB_DrawCirc(const char * c, int x, int y, int r);

TDB_BitMap TDB_LoadBitMap(const char * file);
void TDB_DrawBitMap(TDB_BitMap * bitmap, const char * c, int x, int y);
void TDB_DestroyBitMap(TDB_BitMap * bitmap);

/*// --------------------------------------------------------------------------------------------------*/

void TDB_ShowCursor() {
  TDB_Write("\x1b[?25h", 0, 0);
}

void TDB_HideCursor() {
  TDB_Write("\x1b[?25l", 0, 0); 
}

void TDB_SetCursor(int col, int row) {
  char buffer[256];

  sprintf(buffer, "\x1b[%i;%iH", row, col);
  write(STDOUT_FILENO, buffer, strlen(buffer));
}

TDB_Pos TDB_GetCursor() {
  char col[5];
  char row[5];
  char c;

  write(STDOUT_FILENO, "\x1b[6n", 4);   
  read(STDIN_FILENO, col, 2);

  for (int i = 0; i < 5; i++) {
    if (read(STDIN_FILENO, &c, 1) != 1 || c == ';') { 
      row[i] = '\0';
      break;
    }

    row[i] = c;
  }

  for (int i = 0; i < 5; i++) {
    if (read(STDIN_FILENO, &c, 1) != 1 || c == 'R') { 
      col[i] = '\0';
      break;
    }

    col[i] = c;
  }

  return (TDB_Pos) { atoi(col), atoi(row) }; 
}

void TDB_Write(const char * t, int x, int y) {
  TDB_SetCursor(x, y);
  write(STDOUT_FILENO, t, strlen(t));
}

void TDB_WriteF(int x, int y, const char * t, ...) {
  va_list args;
  char buffer[256];

  va_start(args, t);
  vsprintf(buffer, t, args);
  TDB_Write(buffer, x, y);
  va_end(args);
}

void TDB_Clear() {
  TDB_Write("\x1b[2J\x1b[H", 0, 0);
}

static void beforeExit() {
  #ifdef __unix__
  tcsetattr(STDIN_FILENO, TCSANOW, &_default);
  #endif

  #ifdef _WIN32
  SetConsoleMode(_input, _default);
  #endif

  TDB_ShowCursor();
  TDB_Clear();
}

void TDB_Setup(int blocking) {
  #ifdef __unix__
  tcgetattr(STDIN_FILENO, &_default);
  
  struct termios raw = _default;
  raw.c_iflag &= ~(IXON);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = blocking;
  
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  #endif

  #ifdef _WIN32
  SetConsoleOutputCP(65001);
  _input = GetStdHandle(STD_INPUT_HANDLE);
  _output = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(_input, &_default);
  
  DWORD raw = _default;
  raw &= ~(ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);

  SetConsoleMode(_input, raw);
  #endif

  atexit(beforeExit);
  TDB_HideCursor();
}

int TDB_GetKey() {
  char c; 

  #ifdef _WIN32
  if (!_kbhit()) return 0;
  #endif
  
  if (read(STDIN_FILENO, &c, 1) != 1) return 0;

  if (c == '\x1b') {
    if (read(STDIN_FILENO, &c, 1) != 1) return 0;
    if (read(STDIN_FILENO, &c, 1) != 1) return 0;
    
    if (c == 'A') return TDB_UP;
    if (c == 'B') return TDB_DOWN;
    if (c == 'C') return TDB_RIGHT;
    if (c == 'D') return TDB_LEFT;
    if (c == 'H') return TDB_HOME;
    if (c == 'F') return TDB_END;
  }

  return c;
}

TDB_Pos TDB_GetSize() {
  #ifdef __unix__
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return (TDB_Pos) { w.ws_col, w.ws_row };
  #endif

  #ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO w; 
  GetConsoleScreenBufferInfo(_output, &w);
  
  return (TDB_Pos) { w.dwSize.X, w.dwSize.Y };
  #endif
}

void TDB_DrawLine(const char * c, int x1, int y1, int x2, int y2) {
  for (float i = 0; i < 1; i += 0.01) {
    int px = ceil(x1 + (x2 - x1) * i);
    int py = ceil(y1 + (y2 - y1) * i);
    
    TDB_Write(c, px, py);
  }
}

void TDB_DrawRect(const char * c, int x, int y, int w, int h) {
  TDB_DrawLine(c, x, y, x+w, y);
  TDB_DrawLine(c, x, y, x, y+h);
  TDB_DrawLine(c, x+w, y, x+w, y+h);
  TDB_DrawLine(c, x, y+h, x+w, y+h);
}

void TDB_DrawTria(const char * c, int x, int y, int x1, int y1, int x2, int y2) {
  int cx = x + x1;
  int cy = y + y1;
  int ex = cx + x2;
  int ey = cy + y2;

  TDB_DrawLine(c, x, y,  cx, cy);
  TDB_DrawLine(c, cx, cy, ex, ey);
  TDB_DrawLine(c, ex, ey, x, y);
}

void TDB_DrawCirc(const char * c, int x, int y, int r) {
  for (double a = 0; a < 2 * M_PI; a += 2 * M_PI / 180) {
    int px = round(2 * r * sin(a) + x);
    int py = round(y + r * cos(a));

    TDB_Write(c, px, py);
  }
}

void TDB_DrawASCII(char ** l, int r, int x, int y) {
  for (int py = 0; py < r; py++) {
    TDB_Write(l[py], x, y + py); 
  }
}

TDB_BitMap TDB_LoadBitMap(const char * file) {
  TDB_BitMap bitmap;
  FILE * stream = fopen(file, "r");
  
  unsigned int bit_per_pixel;
  unsigned int offset;

  if (stream == NULL) { fprintf(stderr, "TDB_LoadBitMap | %s | File stream not opened.\n", file); exit(EXIT_FAILURE); }
  if (fgetc(stream) != 'B' || fgetc(stream) != 'M') { fprintf(stderr, "TDB_LoadBitMap | %s | File not is a bitmap.\n", file); exit(EXIT_FAILURE); }

  fseek(stream, 10, SEEK_SET);
  fread(&offset, 4, 1, stream);

  fseek(stream, 4, SEEK_CUR);
  fread(&bitmap.width, 4, 1, stream);
  fread(&bitmap.height, 4, 1, stream);

  fseek(stream, 2, SEEK_CUR);
  fread(&bit_per_pixel, 2, 1, stream);

  bitmap.bytes_per_pixel = bit_per_pixel / 8;
  bitmap.row_size = (bit_per_pixel * bitmap.width + 31) / 32 * 4;
  bitmap.data = (uint8_t *) malloc(bitmap.width * bitmap.height * bitmap.bytes_per_pixel);

  fseek(stream, offset, SEEK_SET);
  fread(bitmap.data, bitmap.bytes_per_pixel, bitmap.width * bitmap.height, stream);

  return bitmap;
}

void TDB_DrawBitMap(TDB_BitMap * bitmap, const char * c, int x, int y) {
  for (int px = 0; px < bitmap->width; px++) {
    for (int py = 0; py < bitmap->height; py++) {
      unsigned int of = bitmap->row_size * py + bitmap->bytes_per_pixel * px;
      if (bitmap->bytes_per_pixel > 3 && bitmap->data[of+3] == 0) continue;

      TDB_WriteF(x + (px * 2), y + bitmap->height - py, "\x1b[38;2;%u;%u;%um%s\x1b[0m\n", bitmap->data[of+2], bitmap->data[of+1], bitmap->data[of], c);
    }
  }
}

void TDB_DestroyBitMap(TDB_BitMap * bitmap) {
  free(bitmap->data);
  bitmap->data = NULL;
}
#endif
