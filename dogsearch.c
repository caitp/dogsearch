#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char* graph[] = {
  "DGOODDODGOODDO",
  "ODOOGGGDODGOGG",
  "OGOGDOODGOODDD",
  "DGDOOOGGOOGDGO",
  "OGDGOGDGOGGOGD",
  "DDDGDDODOOGDOO",
  "ODGOGGDOOGGOOD"
};

const int gridWidth = 14;
const int gridHeight = 7;

const unsigned long M_DOG = 0x00444F47;

unsigned long makeSequence(char x, char y, char z) {
  return ((x << 16) | (y << 8) | z) & 0x00FFFFFF;
}

unsigned long getRowL2R(int x, int y) {
  return makeSequence(graph[y][x], graph[y][x+1], graph[y][x+2]);
}
unsigned long getRowR2L(int x, int y) {
  return makeSequence(graph[y][x+2], graph[y][x+1], graph[y][x]);
}
unsigned long getColT2B(int x, int y) {
  return makeSequence(graph[y][x], graph[y+1][x], graph[y+2][x]);
}
unsigned long getColB2T(int x, int y) {
  return makeSequence(graph[y+2][x], graph[y+1][x], graph[y][x]);
}
unsigned long getDiagTL2BR(int x, int y) {
  return makeSequence(graph[y][x], graph[y+1][x+1], graph[y+2][x+2]);
}
unsigned long getDiagBR2TL(int x, int y) {
  return makeSequence(graph[y+2][x+2], graph[y+1][x+1], graph[y][x]);
}
unsigned long getDiagTR2BL(int x, int y) {
  return makeSequence(graph[y][x+2], graph[y+1][x+1], graph[y+2][x]);
}
unsigned long getDiagBL2TR(int x, int y) {
  return makeSequence(graph[y+2][x], graph[y+1][x+1], graph[y][x+2]);
}

enum Mode { // Mode & 0xFC === Type
            // Mode & 0x03 === Index
  ROW_L2R   = 0x00, // 0x00, 0x01, 0x02, xxxx
  ROW_R2L   = 0x04, // 0x04, 0x05, 0x06, xxxx
  COL_T2B   = 0x08, // 0x08, 0x09, 0x0A, xxxx
  COL_B2T   = 0x0C, // 0x0C, 0x0D, 0x0E, xxxx, xxxx, xxxxx, xxxxx, xxxxx,
  DIA_TL2BR = 0x10, // 0x10, 0x11, xxxx, xxxx, xxxx
  DIA_TR2BL = 0x14, // 0x14, 0x15, xxxx
};

int decodeX(int x, int y, int mode) {
  switch (mode & 0xFC) {
    case ROW_L2R: case ROW_R2L: case DIA_TL2BR: return x;
    case COL_T2B: case COL_B2T: return x + (mode & 0x03);
    case DIA_TR2BL: return x + 2;
    default: exit(1);
  }
  return 0;
}

int decodeY(int x, int y, int mode) {
  switch (mode & 0xFC) {
    case ROW_L2R: case ROW_R2L: return y + (mode & 0x03);
    case COL_T2B: case COL_B2T: case DIA_TL2BR: case DIA_TR2BL: return y;
    default: exit(1);
  }
  return 0;
}

const char* decodeMode(int mode) {
  switch (mode & 0xFC) {
    case ROW_L2R: return "row right -> left";
    case ROW_R2L: return "row left -> right";
    case COL_T2B: return "column top -> bottom";
    case COL_B2T: return "column bottom -> top";
    case DIA_TL2BR:
      if (!(mode & 0x03)) {
        return "diagonal topleft -> bottomright";
      } else {
        return "diagonal bottomright -> topleft";
      }
    case DIA_TR2BL:
      if (!(mode & 0x03)) {
        return "diagonal topright -> bottomleft";
      } else {
        return "diagonal bottomleft -> topright";
      }
    default: exit(1);
  }
  return 0;
}

int checkSection(int x, int y, int n) {
  unsigned long sections[] = {
    /* Rows */
    /* 0x00 */ getRowL2R(x, y),
    /* 0x01 */ getRowL2R(x, y+1),
    /* 0x02 */ getRowL2R(x, y+2),
    /* xxxx */ 0,
    /* 0x04 */ getRowR2L(x, y),
    /* 0x05 */ getRowR2L(x, y+1),
    /* 0x06 */ getRowR2L(x, y+2),
    /* xxxx */ 0,

    /* Columns */
    /* 0x08 */ getColT2B(x, y),
    /* 0x09 */ getColT2B(x+1, y),
    /* 0x0A */ getColT2B(x+2, y),
    /* xxxx */ 0,
    /* 0x0C */ getColB2T(x, y),
    /* 0x0D */ getColB2T(x+1, y),
    /* 0x0E */ getColB2T(x+2, y),
    /* xxxx */ 0,

    /* Diagonals */
    /* 0x10 */ getDiagTL2BR(x, y),
    /* 0x11 */ getDiagBR2TL(x, y),
    /* xxxx */ 0, 0,
    /* 0x14 */ getDiagTR2BL(x, y),
    /* 0x15 */ getDiagBL2TR(x, y)
  };

  for (int i=0; i < sizeof(sections) / sizeof(*sections); ++i) {
    unsigned long chunk = sections[i];
    if (!chunk) continue;

    if (chunk == M_DOG) {
      int rx = decodeX(x, y, i) + 1;
      int ry = decodeY(x, y, i) + 1;
      const char* mode = decodeMode(i);
      printf("%d: %d,%d (%s)\n", ++n, rx, ry, mode);
    }
  }
  return n;
}

int main(void) {
  int n = 0;
  for (int x=0; x < gridWidth - 3; ++x) {
    for (int y=0; y < gridHeight - 3; ++y) {
      n = checkSection(x, y, n);
    }
  }
  printf("Found %d matches.\n", n);
  return 0;
}

