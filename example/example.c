#include "../tdb.h"

int main() {
  TDB_Setup(1); 

  TDB_BitMap sprite = TDB_LoadBitMap("./image.bmp");
 
  TDB_Size ps;

  while (1) {
    int k = TDB_GetKey();
    TDB_Size s = TDB_GetSize();
    if (k == 'q' || k == TDB_UP) break;

    if (s.x != ps.x || s.y != ps.y) {
      TDB_Clear();
      TDB_DrawCirc("\x1b[0;32m$\x1b[0m", 24, 24, 8);
      TDB_DrawRect("\x1b[0;31m#\x1b[0m", 48, 24, 18, 8);
      TDB_DrawTria("\x1b[0;34m@\x1b[0m", 48, 36, 22, 7, -22, 8);
      TDB_DrawBitMap(&sprite, TDB_PIXEL_CIRCLE, (s.col - sprite.width * 2) / 2, (s.row - sprite.height) / 2, 2);
    }

    ps = s;
  }

  TDB_Clear();
  TDB_DestroyBitMap(&sprite);

  return 1;
}
