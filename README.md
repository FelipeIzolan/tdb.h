# 🖌️🎨 tdb.h

![image](https://github.com/FelipeIzolan/tdb.h/assets/80170121/a7df4ca8-db8f-46e6-8451-b1138d2dadc5)

## 🚀 Usage

```c
#include "tdb.h"
#include <stdbool.h>

int main() {
  TDB_Setup(1); // <- 0 to non-blocking

  bool will_render = 1;
  TDB_Size prev_size;

  while (1) {
    int k = TDB_GetKey();
    TDB_Size s = TDB_GetSize();

    if (k == 'q' || k == TDB_UP) break;
    if (s.x != prev_size.x || s.y != prev_size.y) will_render = 1;

    if (will_render) {
      TDB_Clear();
      TDB_SetCursor((s.col - 13) / 2, s.row / 2);
      TDB_Write("Hello, World!");
      will_render = 0;
    }

    prev_size = s;
  }

  TDB_Clear();
  return 1;
}
```

## 📜 License

[tdb.h](https://github.com/FelipeIzolan/tdb.h) - The Unlicemsed
