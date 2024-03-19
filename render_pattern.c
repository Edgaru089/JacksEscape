#include "render_pattern.h"
#include <math.h>


BYTE *render_DissolvePattern(double progress) {
	if (progress < .0)
		progress = .0;
	if (progress > 1.0)
		progress = 1.0;

	int index = round(progress * 65);
	return (BYTE *)(render_DissolvePatternData + index * 8);
}


// generated from test_generate_pattern.cpp
char render_DissolvePatternData[520] =
	"\xff\xff\xff\xff\xff\xff\xff\xff"
	"\x7f\xff\xff\xff\xff\xff\xff\xff"
	"\x3f\xff\xff\xff\xff\xff\xff\xff"
	"\x3f\xbf\xff\xff\xff\xff\xff\xff"
	"\x3f\x3f\xff\xff\xff\xff\xff\xff"
	"\x3f\x3f\x7f\xff\xff\xff\xff\xff"
	"\x3f\x3f\x3f\xff\xff\xff\xff\xff"
	"\x3f\x3f\x1f\xff\xff\xff\xff\xff"
	"\x3f\x1f\x1f\xff\xff\xff\xff\xff"
	"\x1f\x1f\x1f\xff\xff\xff\xff\xff"
	"\x0f\x1f\x1f\xff\xff\xff\xff\xff"
	"\x0f\x0f\x1f\xff\xff\xff\xff\xff"
	"\x0f\x0f\x0f\xff\xff\xff\xff\xff"
	"\x0f\x0f\x0f\xef\xff\xff\xff\xff"
	"\x0f\x0f\x0f\xcf\xff\xff\xff\xff"
	"\x0f\x0f\x0f\x8f\xff\xff\xff\xff"
	"\x0f\x0f\x0f\x0f\xff\xff\xff\xff"
	"\x0f\x0f\x0f\x0f\x7f\xff\xff\xff"
	"\x0f\x0f\x0f\x0f\x3f\xff\xff\xff"
	"\x0f\x0f\x0f\x0f\x1f\xff\xff\xff"
	"\x0f\x0f\x0f\x0f\x0f\xff\xff\xff"
	"\x0f\x0f\x0f\x0f\x07\xff\xff\xff"
	"\x0f\x0f\x0f\x07\x07\xff\xff\xff"
	"\x0f\x0f\x07\x07\x07\xff\xff\xff"
	"\x0f\x07\x07\x07\x07\xff\xff\xff"
	"\x07\x07\x07\x07\x07\xff\xff\xff"
	"\x03\x07\x07\x07\x07\xff\xff\xff"
	"\x03\x03\x07\x07\x07\xff\xff\xff"
	"\x03\x03\x03\x07\x07\xff\xff\xff"
	"\x03\x03\x03\x03\x07\xff\xff\xff"
	"\x03\x03\x03\x03\x03\xff\xff\xff"
	"\x03\x03\x03\x03\x03\xfb\xff\xff"
	"\x03\x03\x03\x03\x03\xf3\xff\xff"
	"\x03\x03\x03\x03\x03\xe3\xff\xff"
	"\x03\x03\x03\x03\x03\xc3\xff\xff"
	"\x03\x03\x03\x03\x03\x83\xff\xff"
	"\x03\x03\x03\x03\x03\x03\xff\xff"
	"\x03\x03\x03\x03\x03\x03\x7f\xff"
	"\x03\x03\x03\x03\x03\x03\x3f\xff"
	"\x03\x03\x03\x03\x03\x03\x1f\xff"
	"\x03\x03\x03\x03\x03\x03\x0f\xff"
	"\x03\x03\x03\x03\x03\x03\x07\xff"
	"\x03\x03\x03\x03\x03\x03\x03\xff"
	"\x03\x03\x03\x03\x03\x03\x01\xff"
	"\x03\x03\x03\x03\x03\x01\x01\xff"
	"\x03\x03\x03\x03\x01\x01\x01\xff"
	"\x03\x03\x03\x01\x01\x01\x01\xff"
	"\x03\x03\x01\x01\x01\x01\x01\xff"
	"\x03\x01\x01\x01\x01\x01\x01\xff"
	"\x01\x01\x01\x01\x01\x01\x01\xff"
	"\x00\x01\x01\x01\x01\x01\x01\xff"
	"\x00\x00\x01\x01\x01\x01\x01\xff"
	"\x00\x00\x00\x01\x01\x01\x01\xff"
	"\x00\x00\x00\x00\x01\x01\x01\xff"
	"\x00\x00\x00\x00\x00\x01\x01\xff"
	"\x00\x00\x00\x00\x00\x00\x01\xff"
	"\x00\x00\x00\x00\x00\x00\x00\xff"
	"\x00\x00\x00\x00\x00\x00\x00\xfe"
	"\x00\x00\x00\x00\x00\x00\x00\xfc"
	"\x00\x00\x00\x00\x00\x00\x00\xf8"
	"\x00\x00\x00\x00\x00\x00\x00\xf0"
	"\x00\x00\x00\x00\x00\x00\x00\xe0"
	"\x00\x00\x00\x00\x00\x00\x00\xc0"
	"\x00\x00\x00\x00\x00\x00\x00\x80"
	"\x00\x00\x00\x00\x00\x00\x00"; // And the last \0 in the string terminator
