
#include "rand.h"
#include <stdlib.h>


double rand_Double01() {
	// Very bad implementation.
	return ((double)rand()) / (double)RAND_MAX;
}


double rand_DoubleRange(double min, double max) {
	return min + rand_Double01() * (max - min);
}
