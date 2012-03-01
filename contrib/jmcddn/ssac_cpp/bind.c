int
static num_phys_cores()
{
#ifdef __APPLE__
  // based on doc I could find on net about OSX/mach internals
  int mib[4], numcores;
  size_t len, size;
  
  len = 4;
  sysctlnametomib("hw.physicalcpu_max", mib, &len);

  size = sizeof(numcores);
  if (sysctl(mib, len, &numcores, &size, NULL, 0)==-1) {
	perror("sysctl");
	return -1;
  }
  return numcores;
#else 
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

#ifndef __APPLE__
struct linux_thread_init_arg {
  void *(*func)(void *);
  void *arg;
};

void *
linux_thread_init(void *arg, int processor)
{
  struct linux_thread_init_arg *a = (struct linux_thread_init_arg *)arg;
  void *(*func)(void *) = a->func;
  void *theArg = a->arg;

  free(arg);

  cpu_set_t mask;
  // Pin process to cpu
  CPU_ZERO( &mask );
  CPU_SET(processor, &mask);
  if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
    perror("ERROR: Could not set CPU Affinity, exiting...\n");
    exit(-1);
  }
  return func(arg);
}
#endif

pthread_t
create_bound_thread(int id, void *(*func)(void *), void *arg);
{
  int numcores, physicalcore, pid, rc;
  thread_affinity_policy_data_t affinityinfo;
  pthread_t tid;

  numcores = num_phys_cores();
  pid = id % numcores;

  if (id < 0 || id >= numcores) return -1;

#ifdef __APPLE__
  rc = pthread_create_suspended_np(&tid, NULL, func, arg);
  if (rc != 0) {
    perror("pthread_create_suspended_np");
    return -1;
  }

  affinityinfo.affinity_tag = pid+1;
  rc = thread_policy_set(pthread_mach_thread_np(tids[id]), 
			 THREAD_AFFINITY_POLICY,
			 &affinityinfo,
			 THREAD_AFFINITY_POLICY_COUNT);
  if (rc != KERN_SUCCESS) {
    perror("thread_policy_set");
    return -1;
  }

  thread_resume(pthread_mach_thread_np(tids[id]));
#else
  // Linux code here
  struct linux_thread_init_arg *args;
  args->func = proc;
  args->arg = arg;
  rc = pthread_create(&tid, NULL, linux_thread_init, (void *)args) < 0);
  if (rc != 0) {
    perror("pthread_create");
    return -1;
  }
#endif
  
  return tid;
}
