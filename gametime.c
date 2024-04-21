
#include "gametime.h"
#include "types.h"


System_GameTime *gametime_NewSystem(App *super) {
	System_GameTime *sys = zero_malloc(sizeof(System_GameTime));

	sys->super = super;
	sys->now   = GAMETIME_EPOCH;
	return sys;
}
void gametime_DeleteSystem(System_GameTime *sys) {
	free(sys);
}

void gametime_Advance(System_GameTime *sys, Duration deltaTime) {
	sys->now += deltaTime.microseconds;
}


TimePoint gametime_Now(System_GameTime *sys) {
	TimePoint now = {.microseconds = sys->now};
	return now;
}

Duration gametime_Since(System_GameTime *sys, TimePoint prev) {
	return time_Difference(gametime_Now(sys), prev);
}

Duration gametime_Reset(System_GameTime *sys, TimePoint *prev) {
	TimePoint now = gametime_Now(sys);
	Duration  d   = time_Difference(now, *prev);
	*prev         = now;
	return d;
}
