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
# 15 "objtest.c"
struct Obj_if_struct {
  const char *(*name) (void *_data);
  uval (*init) (void *_data);
};

struct Counter_if_struct {
  void (*inc) (void *_data);
  void (*dec) (void *_data);
  uval (*val) (void *_data);
};

struct File_if_struct {
  uval (*open) (void *_data, char *name, uval mode);
  uval (*close) (void *_data);
  uval (*read) (void *_data, char *buf, uval len);
  uval (*write) (void *_data, char *buf, uval len);
};

struct MyObj_obj {
  struct vtable_struct
    {
      struct Obj_if_struct Obj_if;
      struct Counter_if_struct Counter_if;
      struct File_if_struct File_if;
    }
  vtable;
  uval val;
  uval fd;
  uval offset;
};

int
main(int argc, char **argv)
{
  struct MyObj o;

  o.init();
  o.inc();
  o.open("testFile", 0);
}
