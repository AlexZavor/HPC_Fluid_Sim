// Timing class used for some timing and benchmarking 
// This code was writen by GitHub Copilot and then edited by me.

#pragma once

// Reset internal timer (sets "last" to now)
void timing_reset(void);

// Returns delta time in seconds since last call to timing_getDelta() or timing_reset().
// Also advances the internal "last" timestamp.
double timing_getDelta(void);

// Sleep for the specified number of seconds (fractional allowed)
void timing_sleepSeconds(double seconds);

