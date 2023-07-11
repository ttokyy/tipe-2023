#ifndef CANVAS_H
#define CANVAS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <SDL.h>

#include "tools.h"
#include "linked_list.h"



// ----- STRUCTURE DEFINITIONS ----- //

struct cell_s {
	SDL_Rect fill_square; // rectangle for color filling - contains pixel coordinates
	double attractiveness;
	int service_presence_class; // -1 if unoccupied by a service
	int entertain_presence_type;

	double terrain_height;
};

struct vcanvas_s {
	int width_cells; // width in number of cells
	int height_cells; // height in number of cells
	int cell_size; // on-screen size of cells in pixels

	struct cell_s** cellsA;
	int* sorted_cell_indices_attractiveness; // by default, is equal to the identity array - the sorting
							  // is done according to attractivenesses and indices
							  // correspond to the linearized positions of cells in
							  // cellsA
	l_list service_border_pixelsL; // midpoints of service boundaries
	l_list service_interior_cellsL;
};

typedef struct cell_s cell;
typedef struct vcanvas_s* canvas;


// ----- GLOBAL CONSTANTS ----- //

extern const int cCellSizePixels;
extern const int cCellHomeLimit;

extern const int cNumServiceClasses;
extern const int cNumEntertainTypes;
extern const double cServiceWeights[7];

extern const double cDistanceScaleFactor; // factor by which


extern const int cWindowDims[2];


// ----- FUNCTIONS ----- //

// creating canvases
canvas CREATE_CANVAS (int CellSizePixels, const int WindowDims[2]);

// handy functions
int CANVAS_HEIGHT (canvas Canvas, const int WindowDims[2]);
int CANVAS_WIDTH (canvas Canvas, const int WindowDims[2]);
int LIN_COEFF (canvas Canvas, int row, int column);
void ASSIGN_DELIN_INDEX (int* prow, int* pcolumn, canvas Canvas, int index);

// cell-pixel correspondece
void ASSIGN_MIDPOINT_OF_CELL (int* pX, int* pY, canvas Canvas, int row, int column);
void ASSIGN_CELL_OF_PIXEL (int* prow, int* pcolumn, canvas Canvas, int X, int Y);

// cell tests
bool IS_CELL_IN_CANVAS (canvas Canvas, int i, int j);
bool ARE_NEIGHBOR_CELLS (canvas Canvas, int i1, int j1, int i2, int j2);

// ensuring correctness
void SORT_CELL_INDICES (canvas Canvas);

// calculation functions
double F_CLASS_PLACEMENT_SCORE (double SquareDist, int class);

// calculating canvas attributes
void CALCULATE_ATTRACTIVENESSES (canvas Canvas);

// quicksort
void SWAP (int* IntTab, double* DoubleTab, int pos1, int pos2);
int QS_PARTITION_CUT (int* CellIndices, double* CellScores, int start, int end, int pivot);
void QS_SORT_ELEMENTS_BY_SCORE (int* CellIndices, double* CellScores, int start, int end);


#endif