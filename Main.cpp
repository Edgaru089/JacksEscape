#include <graphics.h>
#include <stdio.h>

#include "Types.h"


int main() {
	TimePoint startup = time_Now();

	initgraph(640, 480);
	circle(200, 200, 100);
	Sleep(5000);
	closegraph();

	printf("%.6lf seconds has elapsed\n", duration_Seconds(time_Since(startup)));
	return 0;
}
