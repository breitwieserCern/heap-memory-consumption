#define _GNU_SOURCE

// based on: https://stackoverflow.com/questions/6083337/overriding-malloc-using-the-ld-preload-mechanism
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
   int heap_size; 
   int offset;
   int peak_heap_size;
} MemTrace;

static MemTrace kMemTrace = {0, 0, 0};
static unsigned kPtrSize = sizeof(int*);

void IncreaseHeapSize(int size) {
  kMemTrace.heap_size += size; 
  int diff = kMemTrace.heap_size - kMemTrace.offset;
  if (diff > kMemTrace.peak_heap_size) {
    kMemTrace.peak_heap_size = diff;
  }
}

void DecreaseHeapSize(int size) {
  kMemTrace.heap_size -= size; 
}

void StartPeakHeapSizeMeasurement() {
   kMemTrace.peak_heap_size = 0;
   kMemTrace.offset = kMemTrace.heap_size;
}

int GetPeakHeapSize() {
  return kMemTrace.peak_heap_size;
}

char tmpbuff[10240];
unsigned long tmppos = 0;
unsigned long tmpallocs = 0;

void *memset(void *, int, size_t);
void *memmove(void *to, const void *from, size_t size);

/*=========================================================
 * interception points
 */

static void *(*real_calloc)(size_t nmemb, size_t size);
static void *(*real_malloc)(size_t size);
static void (*real_free)(void *ptr);
static void *(*real_realloc)(void *ptr, size_t size);
static void *(*real_memalign)(size_t blocksize, size_t bytes);

static void init() {
  real_malloc = dlsym(RTLD_NEXT, "malloc");
  real_free = dlsym(RTLD_NEXT, "free");
  real_calloc = dlsym(RTLD_NEXT, "calloc");
  real_realloc = dlsym(RTLD_NEXT, "realloc");
  real_memalign = dlsym(RTLD_NEXT, "memalign");

  if (!real_malloc || !real_free || !real_calloc || !real_realloc ||
      !real_memalign) {
    fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    exit(1);
  }
}

void *malloc(size_t size) {
  static int initializing = 0;
  if (real_malloc == NULL) {
    if (!initializing) {
      initializing = 1;
      init();
      initializing = 0;
    } else {
      if (tmppos + size < sizeof(tmpbuff)) {
        void *retptr = tmpbuff + tmppos;
        tmppos += size;
        ++tmpallocs;
        return retptr;
      } else {
        fprintf(stdout,
                "jcheck: too much memory requested during initialisation - "
                "increase tmpbuff size\n");
        exit(1);
      }
    }
  }
  IncreaseHeapSize(size);
  void *ptr = real_malloc(size + kPtrSize);
  *((size_t*) ptr) = size;
  return ptr + kPtrSize;
}

void free(void *ptr) {
  if (ptr >= (void *)tmpbuff && ptr <= (void *)(tmpbuff + tmppos)) {
    fprintf(stdout, "freeing temp memory\n");
  } else {
    ptr -= kPtrSize;
    size_t size = *((size_t*) ptr);
    DecreaseHeapSize(size);
    real_free(ptr);
  }
}

void *realloc(void *ptr, size_t size) {
  if (real_malloc == NULL) {
    void *nptr = malloc(size);
    if (nptr && ptr) {
      memmove(nptr, ptr, size);
      free(ptr);
    }
    return nptr;
  }

  void *nptr = real_realloc(ptr, size);
  return nptr;
}

void *calloc(size_t nmemb, size_t size) {
  if (real_malloc == NULL) {
    void *ptr = malloc(nmemb * size);
    if (ptr) memset(ptr, 0, nmemb * size);
    return ptr;
  }

  void *ptr = real_calloc(nmemb, size);
  return ptr;
}

void *memalign(size_t blocksize, size_t bytes) {
  void *ptr = real_memalign(blocksize, bytes);
  return ptr;
}
