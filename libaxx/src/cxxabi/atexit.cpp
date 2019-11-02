/* The __cxa_atexit function registers a function to be called when the program
 * exits or when a shared library is unloaded.
 * We don't support shared libraries and our program should never exit, so we
 * can simply do nothing and return zero. */

extern "C" int __cxa_atexit(void (*dtor)(void *), void * object, void * handle) {
  return 0;
}
