#include "timing.hpp"
#include <time.h>

static struct timespec last_ts;
static int initialized = 0;

void timing_reset(void) {
    clock_gettime(CLOCK_MONOTONIC, &last_ts);
    initialized = 1;
}

double timing_getDelta(void) {
    struct timespec now;
    if (!initialized) {
        timing_reset();
    }
    clock_gettime(CLOCK_MONOTONIC, &now);
    double dt = (now.tv_sec - last_ts.tv_sec) + (now.tv_nsec - last_ts.tv_nsec) / 1e9;
    last_ts = now;
    return dt;
}

void timing_sleepSeconds(double seconds) {
    if (seconds <= 0.0) return;
    struct timespec req;
    req.tv_sec = (time_t)seconds;
    req.tv_nsec = (long)((seconds - (time_t)seconds) * 1e9);
    nanosleep(&req, NULL);
}
