#include "../tdb.h"

int main() {
  TDB_Setup(1);

  TDB_Pos ps;
  TDB_BitMap image = TDB_LoadBitMap("./image.bmp");

  while (1) {
    TDB_Pos s = TDB_GetSize();

    #ifdef _WIN32
    if (GetKeyState(0x51) || GetKeyState(VK_UP)) break;
    #else
    int k = TDB_GetKey();
    if (k == 'q' || k == TDB_UP) break;
    #endif

    if (s.col != ps.col || s.row != ps.row) {
      TDB_Clear();
      TDB_DrawBitMap(&image, TDB_PIXEL_CIRCLE, (s.col - image.width * 2) / 2, (s.row - image.height) / 2);
      TDB_Write("I use arch btw!", 2, 0);
    }

    ps = s;
  }

  TDB_Clear();
  TDB_DestroyBitMap(&image);

  return 1;
}
