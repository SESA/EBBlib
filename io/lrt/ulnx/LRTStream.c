#include <io/Stream.h>

EBBType(LRTStream) {
  CObjImplements(EventHandler);
  CObjImplements(Stream);
  int fd;
};

static EBBRC
LRTConsole_handleEvent(EventHandlerRef _self) 
{   
}

static EBBRC
LRTConsole_read(ConsoleStreamRef _self) 
{
  return read(self->fd, 
}

static EBBRC
LRTConsole_write(ConsoleStreamRef _self) 
{
}

static EBBRC
LRTConsole_flush(ConsoleStreamRef _self) 
{
}

static EBBRC
LRTConsole_close(ConsoleStreamRef _self) 
{
}


LRTStreamCreate(int fd) 
{
  
}
