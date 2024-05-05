
#include "render_bundle.h"
#include "render_util.h"
#include "util/vector.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#define RGB(r, g, b) ((r) | ((g) << 8) | ((b) << 16))


vector_Vector           *render_Bundles;
static render_Bundle    *_tmpb;
static render_Primitive *_tmpp;

// Subsequent tokens can be read by strtok(NULL, " ")
static void _render_BundleCommand(char *cmd) {
	// convert CMD to upper case
	for (int i = 0; i < strlen(cmd); i++)
		cmd[i] = toupper(cmd[i]);

#define CMD(str) (strcmp(cmd, str) == 0)
	// Find the command
	if (CMD("BUNDLE")) {
		// Start a bundle
		if (_tmpb != NULL) {
			WARN("_tmpb!=0, you forgot to use ENDBUNDLE for the last bundle, discarding");
			free(_tmpb);
		}
		_tmpb = render_NewBundle(strtok(NULL, " "));


	} else if (CMD("ENDBUNDLE")) {
		// commit the bundle
		if (_tmpb) {
			fprintf(stderr, "[_render_BundleCommand] Commiting bundle %s with %d primitives\n", _tmpb->name, vector_Size(_tmpb->prims));
			vector_Push(render_Bundles, &_tmpb);
			_tmpb = NULL;
		} else
			WARN("_tmpb==0, ENDBUNDLE without BUNDLE first");


	} else if (CMD("PRIM")) {
		// start a primitive
		if (_tmpp != NULL) {
			WARN("_tmpp!=0, you forgot to ENDPRIM for the last primitive, discarding");
			if (_tmpp->points)
				vector_Destroy(_tmpp->points);
			free(_tmpp);
		}
		_tmpp         = malloc(sizeof(render_Primitive));
		_tmpp->points = vector_Create(sizeof(Vec2));
		_tmpp->mode   = render_ModeDefault;

		// parse the type
		char *type = strtok(NULL, " ");
		if (strcmp(type, "LINES") == 0)
			_tmpp->type = render_Lines;
		else if (strcmp(type, "LINESTRIP") == 0)
			_tmpp->type = render_LineStrip;
		else if (strcmp(type, "POLY") == 0)
			_tmpp->type = render_Polygon;


	} else if (CMD("ENDPRIM")) {
		// commit the primitive
		if (_tmpb && _tmpp) {
			fprintf(stderr, "[_render_BundleCommand] Commiting primitive type %d with %d points\n", _tmpp->type, vector_Size(_tmpp->points));
			vector_Push(_tmpb->prims, _tmpp);
			free(_tmpp);
			_tmpp = NULL;
		} else
			WARN("tmpb=%x, tmpp=%x, ENDPRIM when one of them is NULL", _tmpb, _tmpp);


	} else if (CMD("P")) {
		// Add a Vec2
		if (_tmpp) {
			double x = strtod(strtok(NULL, " "), NULL);
			double y = strtod(strtok(NULL, " "), NULL);
			Vec2   v = vec2(x, y);
			vector_Push(_tmpp->points, &v);
		} else
			WARN("P without PRIM first");
	} else if (CMD("FG")) {
		// Set Foreground color
		if (_tmpp) {
			int r          = atoi(strtok(NULL, " "));
			int g          = atoi(strtok(NULL, " "));
			int b          = atoi(strtok(NULL, " "));
			_tmpp->mode.fg = RGB(r, g, b);
		} else
			WARN("FG without PRIM first");
	} else if (CMD("BG")) {
		// Set Background color
		if (_tmpp) {
			int r          = atoi(strtok(NULL, " "));
			int g          = atoi(strtok(NULL, " "));
			int b          = atoi(strtok(NULL, " "));
			_tmpp->mode.bg = RGB(r, g, b);
		} else
			WARN("BG without PRIM first");
	} else {
		WARN("unknown command %s", cmd);
	}
#undef CMD
}


void render_LoadBundle(const char *filename) {
	if (!render_Bundles)
		render_Bundles = vector_Create(sizeof(render_Bundle *));

	FILE *f = fopen(filename, "r");
	if (!f) {
		WARN("failed to open file\"%s\"", filename);
		return;
	}

	char linebuf[512];
	memset(linebuf, 0, sizeof(linebuf));
	while (!feof(f) && fgets(linebuf, sizeof(linebuf) - 1, f)) {
		while (linebuf[0] !='\0' && linebuf[strlen(linebuf) - 1] == '\n')
			linebuf[strlen(linebuf) - 1] = '\0';

		char *cmd = strtok(linebuf, " ");
		if (cmd == NULL)
			continue;
		_render_BundleCommand(cmd);
	}
}

render_Bundle *render_FindBundle(const char *name) {
	if (!render_Bundles) {
		WARN("called with render_Bundles=NULL (name=%s)", name);
		return NULL;
	}

	// brute force search. could be slow
	for (int i = 0; i < vector_Size(render_Bundles); i++) {
		render_Bundle *b = *(render_Bundle **)vector_At(render_Bundles, i);
		if (strcmp(name, b->name) == 0)
			return b;
	}

	WARN("bundle \"%s\" not found", name);
	return NULL;
}
