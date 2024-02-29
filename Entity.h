#pragma once

#include <stdint.h>
#include "util/vector.h"


#ifdef __cplusplus
extern "C" {
#endif


// Entity.
typedef struct {
	uintptr_t type;
	const char *name;

} Entity;



#ifdef __cplusplus
}
#endif
