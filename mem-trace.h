
typedef struct {
   int mem_cnt_; 
   int offset_;
   int peak_mem_cnt_;
} MemTrace;

extern MemTrace kMemTrace;

void MemTraceInit() {
  kMemTrace.mem_cnt_ = 0;
  kMemTrace.offset_ = 0;
  kMemTrace.peak_mem_cnt_ = 0;
}

void Allocate(int size) {
  kMemTrace.mem_cnt_ += size; 
  int diff = kMemTrace.mem_cnt_ - kMemTrace.offset_;
  if (diff < kMemTrace.peak_mem_cnt_) {
    kMemTrace.peak_mem_cnt_ = diff;
  }
}

void Deallocate(int size) {
  kMemTrace.mem_cnt_ -= size; 
}

void ResetPeakMemCnt() {
   kMemTrace.peak_mem_cnt_ = 0;
   kMemTrace.offset_ = 0;
}

int GetPeakMemUsage() {
  return kMemTrace.peak_mem_cnt_;
}

