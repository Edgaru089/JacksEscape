#include <graphics.h>


int main() {
	initgraph(640, 480);
	circle(200, 200, 100);
	Sleep(5000);
	closegraph();
	return 0;
}
