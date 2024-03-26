#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// returns a double in the [0,1) range.
double rand_Double01();

double rand_DoubleRange(double min, double max);

#ifdef __cplusplus
}
#endif
