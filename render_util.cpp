
#include "render_util.h"
#include "easyx.h"
#include "util/vector.h"
#include <string.h>
#include <stdio.h>


extern "C" {


static vector_Vector *tbuf;

void render_DrawText(int x, int y, const char *str) {
	if (!tbuf)
		tbuf = vector_Create(1);

	int        cx = x, cy = y;
	const char zero = 0;

	vector_Clear(tbuf);
	int len = strlen(str);
	printf("%s, len=%d\n", str, len);
	int i = 0;
	while (i < len) {
		if (str[i] == '\n') {
			vector_Push(tbuf, &zero);
			printf("outtext: \"%s\"\n", vector_Data(tbuf));
			outtextxy(cx, cy, (LPCTSTR)vector_Data(tbuf));

			cy += TEXTHEIGHT;
			vector_Clear(tbuf);
		} else
			vector_Push(tbuf, &str[i]);
		i++;
	}

	if (vector_Size(tbuf) > 0) {
		vector_Push(tbuf, &zero);
		outtextxy(cx, cy, (LPCTSTR)vector_Data(tbuf));
		vector_Clear(tbuf);
	}
}
}
