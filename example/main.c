#include "../tdb.h"

int main() {
  TDB_Setup(1);

  TDB_Size ps;
  TDB_BitMap image = TDB_LoadBitMap("./image.bmp");

  while (1) {
    int k = TDB_GetKey();
    TDB_Size s = TDB_GetSize();

    if (k == 'q' || k == TDB_UP) break;
    if (s.x != ps.x || s.y != ps.y) {
      TDB_Clear();
      TDB_DrawBitMap(&image, TDB_PIXEL_CIRCLE, (s.col - image.width * 2) / 2, (s.row - image.height) / 2, 2);
      TDB_Write("I use arch btw!", 2, 0);
    }

    ps = s;
  }

  TDB_Clear();
  TDB_DestroyBitMap(&image);

  return 1;
}
