
EBBImp(ConsoleStream, Imp) {
  char *buf;
  int   bufsize;
};

static EBBRC
ConsoleStreamImp_handleEvent(EventHandlerRef _self) {}

static EBBRC
ConsoleStreamImp_read(ConsoleStreamRef _self) {}

static EBBRC
ConsoleStreamImp_write(ConsoleStreamRef _self) {}

static EBBRC
ConsoleStreamImp_flush(ConsoleStreamRef _self) {}

static EBBRC
ConsoleStreamImp_close(ConsoleStreamRef _self) {}

static void
initStream(StreamId id, lrt_pic_source src)
{
  uintptr_t evnum;

  rc = EBBCALL(theEventMgrPrimId, allocEventNo, &evnum);
  EBBRCAssert(rc);

  rc = EBBCALL(theEventMgrPrimId, registerHandler, evnum, id, insrc);  
  EBBRCAssert(rc);
}

static void
initStreams(StreamId inId, StreamId outId, StreamId err)
{
  lrt_pic_source insrc; 
  lrt_pic_source outsrc;
  lrt_pic_source errsrc;

  rc = lrt_console_init(&insrc, &outsrc, &errsrc);
  EBBRCAssert(rc);
  
  initStream(inId,  insrc);
  initStream(outId, outsrc);
  initStream(errId, errsrc);  
}  

EBBRC
ConsoleCreate(StreamId *stdinId, StreamId *stdoutId, StreamId *stderr)
{
  
}  
