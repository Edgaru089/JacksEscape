
#include "rand.h"
#include <stdlib.h>


double rand_Double01() {
	// Very bad implementation.
	return ((double)rand()) / (double)RAND_MAX;
}
