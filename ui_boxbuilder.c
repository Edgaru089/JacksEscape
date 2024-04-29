
#include "types.h"
#include "ui.h"
#include "util/vector.h"

ui_BoxBuilder *ui_BoxBuilder_New() {
	ui_BoxBuilder *builder = zero_malloc(sizeof(ui_BoxBuilder));
	builder->line_height   = UI_BOXBUILDER_DEFAULT_LINEHEIGHT;
	builder->width         = UI_BOXBUILDER_DEFAULT_WIDTH;
	builder->pivot         = vec2(UI_BOXBUILDER_DEFAULT_PIVOT_X, UI_BOXBUILDER_DEFAULT_PIVOT_Y);
	builder->row_counts    = vector_Create(sizeof(int));
	// builder->rows          = vector_Create(sizeof(vector_Vector *));
	return builder;
}
void ui_BoxBuilder_Delete(ui_BoxBuilder *builder) {
	vector_Destroy(builder->row_counts);
	for (int i = 0; i < vector_Size(builder->rows); i++) {
		vector_Vector *vr = *((vector_Vector **)vector_At(builder->rows, i));
		if (vr)
			vector_Destroy(vr);
	}
	vector_Destroy(builder->rows);
	free(builder);
}

// Set that this row should have this many columns
void ui_BoxBuilder_SetCount(ui_BoxBuilder *builder, int row, int num_cols) {
	if (vector_Size(builder->row_counts) < row) {
		size_t old_size = vector_Size(builder->row_counts);
		vector_Resize(builder->row_counts, row);
		// Set the new parts to zero
		for (int i = old_size; i < row; i++)
			*((int *)vector_At(builder->row_counts, i)) = 0;
	}
	*((int *)vector_At(builder->row_counts, row - 1)) = num_cols;
}

// Assemble everything
void ui_BoxBuilder_Assemble(ui_BoxBuilder *builder) {
	if (builder->rows) {
		// Free the previous vectors
		for (int i = 0; i < vector_Size(builder->rows); i++) {
			vector_Vector *vr = *((vector_Vector **)vector_At(builder->rows, i));
			if (vr)
				vector_Destroy(vr);
		}
		vector_Clear(builder->rows);
	} else
		builder->rows = vector_Create(sizeof(vector_Vector *));
	vector_Resize(builder->rows, vector_Size(builder->row_counts));

	double cursor_y = 0.0;
	for (int i = 0; i < vector_Size(builder->row_counts); i++) {
		int row_count = *((int *)vector_At(builder->row_counts, i));
		if (row_count == 0) {
			*(vector_Vector **)vector_At(builder->rows, i) = NULL;
			cursor_y += UI_PADDING;
			continue;
		}
		vector_Vector *row = vector_Create(sizeof(Box2));

		*(vector_Vector **)vector_At(builder->rows, i) = row;

		// Append every box
		double total_width = builder->width - UI_PADDING * (row_count - 1);
		double avg_width   = total_width / row_count;
		for (int i = 0; i < row_count; i++) {
			Box2 box = {
				.lefttop = {.x = i * (avg_width + UI_PADDING), .y = cursor_y},
				.size    = {.x = avg_width, .y = builder->line_height}};
			vector_Push(row, &box);
		}

		cursor_y += builder->line_height + UI_PADDING;
	}

	int  row_num  = vector_Size(builder->row_counts);
	Vec2 max_size = {
		.x = builder->width,
		.y = builder->line_height * row_num + UI_PADDING * (row_num - 1)};
	Box2 max_box = box2_FromPivot(builder->position, builder->pivot, max_size);

	// Move every box
	for (int i = 0; i < vector_Size(builder->row_counts); i++) {
		int row_count = *((int *)vector_At(builder->row_counts, i));
		if (row_count == 0)
			continue;
		vector_Vector *row = *(vector_Vector **)vector_At(builder->rows, i);
		for (int i = 0; i < row_count; i++) {
			Box2 *box    = (Box2 *)vector_At(row, i);
			box->lefttop = vec2_Add(box->lefttop, max_box.lefttop);
		}
	}
}

// Every number starts from 1
Box2 ui_BoxBuilder_At(ui_BoxBuilder *builder, int row, int col) {
	return *(Box2 *)vector_At(*(vector_Vector **)vector_At(builder->rows, row - 1), col - 1);
}
