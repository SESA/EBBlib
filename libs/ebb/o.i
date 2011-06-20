# 1 "objtest.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "objtest.c"
# 1 "../types.h" 1
# 33 "../types.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned long uval;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef long sval;
# 52 "../types.h"
typedef uval EL;
typedef uval ELNum;
# 2 "objtest.c" 2
# 14 "objtest.c"
(name) struct name_if (Obj) {
  const char *(*name) (void *_data);
  uval (*init) (void *_data);
};

(name) struct name_if (Counter) {
  void (*inc) (void *_data);
  void (*dec) (void *_data);
  uval (*val) (void *_data);
};

(name) struct name_if (File) {
  uval (*open) (void *_data, char *name, uval mode);
  uval (*close) (void *_data);
  uval (*char *buf) (void *_data, uval len);
  uval (*write) (void *_data, char *buf, uval len);
};

struct MyObj_obj {
  struct vtable {
    struct Obj_if;
    struct Counter_if;
    struct File_if;
  }
  Data {
    uval val;
    uval fd;
    uval offset
  }
};

int
main(int argc, char **argv)
{
  MyObj o;

  o.init();
  o.inc();
  o.open("testFile", 0);
}
