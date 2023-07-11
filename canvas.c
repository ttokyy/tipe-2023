#include "canvas.h"

// constants
const int cCellSizePixels = 10;
const int cNumServiceClasses = 7;
const int cNumEntertainTypes = 2;

const int cCellHomeLimit = 10;
const double cServiceWeights[7] = {1.2, 0.9, 0.6, 0.4, 0.3, -0.5, -1};

const double cDistanceScaleFactor = 2.0;



// returns an empty canvas (i.e. with all attributes initialized to their null
// or neutral value) covering the entire screen, and whose cells are sized
// CellSizePixels in size
canvas CREATE_CANVAS (int CellSizePixels, const int WindowDims[2]){

	canvas Canvas = (canvas) malloc (sizeof(struct vcanvas_s));

	Canvas->width_cells = ceil (WindowDims[0]/cCellSizePixels);
	Canvas->height_cells = CANVAS_HEIGHT (Canvas, WindowDims);
	Canvas->cell_size = CellSizePixels;

	Canvas->service_border_pixelsL = NULL;
	Canvas->service_interior_cellsL = NULL;

	Canvas->cellsA = (cell**) malloc (Canvas->height_cells*sizeof(cell*));
	Canvas->sorted_cell_indices_attractiveness = (int*) malloc
	/**/ (Canvas->height_cells*Canvas->width_cells*sizeof(int));

	for (int index = 0; index < Canvas->height_cells*Canvas->width_cells; index++)
		Canvas->sorted_cell_indices_attractiveness[index] = index;

	for (int i = 0; i < Canvas->height_cells; i++){

		Canvas->cellsA[i] = (cell*) malloc (Canvas->width_cells*sizeof(cell));

		for (int j = 0; j < Canvas->width_cells; j++){

			Canvas->cellsA[i][j].attractiveness = 0;
			Canvas->cellsA[i][j].service_presence_class = -1;

			Canvas->cellsA[i][j].terrain_height = -DBL_MAX;

			Canvas->cellsA[i][j].fill_square.x = j*cCellSizePixels;
			Canvas->cellsA[i][j].fill_square.y = i*cCellSizePixels;
			Canvas->cellsA[i][j].fill_square.w = cCellSizePixels;
			Canvas->cellsA[i][j].fill_square.h = cCellSizePixels;

		}
	}
	
	return Canvas;
}


/* hyp: Canvas->width_cells has been initialized */
// returns the appropriate value of Canvas->width_cells according
// to Canvas->height_cells and the window size as given by WindowDims
int CANVAS_HEIGHT (canvas Canvas, const int WindowDims[2]){

	return (int) ((((double) WindowDims[1])/((double) WindowDims[0])) * ((double) Canvas->width_cells));
}


/* precond: Canvas->height_cells has been initialized */
// returns the appropriate value of Canvas->width_cells according
// to Canvas->height_cells and the window size as given by WindowDims
int CANVAS_WIDTH (canvas Canvas, const int WindowDims[2]){

	return (int) ((((double) WindowDims[1])/((double) WindowDims[0])) * ((double) Canvas->height_cells)); 
}


int LIN_COEFF (canvas Canvas, int row, int column){

	return row*Canvas->width_cells + column;
}


void ASSIGN_DELIN_INDEX (int* prow, int* pcolumn, canvas Canvas, int index){
	
	*prow = index/Canvas->width_cells;
	*pcolumn = index % Canvas->width_cells;
}


// assigns the on-screen pixel coordinates of cell Canvas->cellsA[row][column]'s
// midpoint to *pX and *pY
void ASSIGN_MIDPOINT_OF_CELL (int* pX, int* pY, canvas Canvas, int row, int column){

	*pX = ((2*column)*Canvas->cell_size)/2;
	*pY = ((2*row)*Canvas->cell_size)/2;
}


// modifies *prow and *pcolumn such that Canvas->cellsA[*prow][*pcolumn] contains
// the on-screen pixel of coordinates (X,Y)
void ASSIGN_CELL_OF_PIXEL (int* prow, int* pcolumn, canvas Canvas, int X, int Y){

	*prow = Y/Canvas->cell_size;
	*pcolumn = X/Canvas->cell_size;
}


// tests whether or not (i,j) forms a valid cell index for Canvas
bool IS_CELL_IN_CANVAS (canvas Canvas, int i, int j){

	return (i >= 0 && i < Canvas->height_cells && j >= 0 && j < Canvas->width_cells);
}


/* hyp: IS_CELL_IN_CANVAS (Canvas, i1, j1) && IS_CELL_IN_CANVAS (Canvas, i2, j2) */
// tests whether or not cells
bool ARE_NEIGHBOR_CELLS (canvas Canvas, int i1, int j1, int i2, int j2){
	
	assert (IS_CELL_IN_CANVAS (Canvas, i1, j1) && IS_CELL_IN_CANVAS (Canvas, i2, j2));
	return (i1 >= i2-1 && i1 <= i2+1 && j1 >= j2-1 && j1 <= j2+1);
}


// updates Canvas->sorted_cell_indices such that the sequence
// (Canvas->cellsA[ik][jk].attractiveness), where LIN (Canvas, ik, jk) = k, increases
// with k
void SORT_CELL_INDICES (canvas Canvas){

	int NumCells = Canvas->width_cells*Canvas->height_cells;
	int* CellIndices = Canvas->sorted_cell_indices_attractiveness;
	double* CellAttractivenesses = (double*) malloc (NumCells*sizeof(double));

	for (int index = 0; index < NumCells; index++){

		int ci, cj;
		ASSIGN_DELIN_INDEX (&ci, &cj, Canvas, CellIndices[index]);
		CellAttractivenesses[index] = Canvas->cellsA[ci][cj].attractiveness;
	}

	QS_SORT_ELEMENTS_BY_SCORE (CellIndices, CellAttractivenesses, 0, NumCells-1);

	free (CellAttractivenesses);
}

// hyp: 0 <= class < cNumServiceClasses
// calculates the placement score contribution of service class class
// for a cell at distance sqrt(SquareDist) away from said class
double F_CLASS_PLACEMENT_SCORE (double SquareDist, int class){
	double Dist = sqrt (SquareDist);
	if (cServiceWeights[class] > 0)
		return (0.02*cServiceWeights[class]*(10.+(Dist/2.))*exp(-(cServiceWeights[class]*SquareDist)/12000.));
	else
		return (0.6*cServiceWeights[class]*exp((cServiceWeights[class]*SquareDist)/20000.));
}


// assigns to each Canvas->cellsA[i][j] its normalized attractiveness value
// (calculated by summing the cell's class placement scores over all classes
// and then normalizing by the attractiveness score of a hypothetical cell whose
// class placement scores are maximal for each class)
void CALCULATE_ATTRACTIVENESSES (canvas Canvas){

	// array that will contain, for each (i,j), Canvas->cellsA[i][j]'s distance to each class'
	double* SqDistsToClasses = (double*) malloc (cNumServiceClasses*sizeof(double));

	double Normalizer = 0;
	for (int class = 0; class < cNumServiceClasses; class++){
		if (cServiceWeights[class] > 0){
			double ArgmaxScore = (10.*(sqrt(1+(60./cServiceWeights[class]))-1))*(10.*(sqrt(1+(60./cServiceWeights[class]))-1));
			Normalizer += F_CLASS_PLACEMENT_SCORE ((int) ArgmaxScore, class);
		}
	}

	for (int i = 0; i < Canvas->height_cells; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			// only score unoccupied cells
			if (Canvas->cellsA[i][j].service_presence_class == -1){

				int CellX = Canvas->cellsA[i][j].fill_square.x;
				int CellY = Canvas->cellsA[i][j].fill_square.y;

				// reset array for current cell's distance calculations
				for (int class = 0; class < cNumServiceClasses; class++){
					SqDistsToClasses[class] = DBL_MAX;
				}

				// calculate distance to each class by looking at all service borders and updating array
				l_list ServiceBorders = Canvas->service_border_pixelsL;

				while (ServiceBorders != NULL){

					int class = ServiceBorders->spec;
					double SqDist = ((ServiceBorders->x - CellX)*(ServiceBorders->x - CellX)
					/**/ + (ServiceBorders->y - CellY)*(ServiceBorders->y - CellY))
					/**/ /cDistanceScaleFactor;
					SqDistsToClasses[class] = min_double (SqDist, SqDistsToClasses[class]);

					ServiceBorders = ServiceBorders->next;
				}

				// calculate attractiveness
				Canvas->cellsA[i][j].attractiveness = 0;
				for (int class = 0; class < cNumServiceClasses; class++){
					if (SqDistsToClasses[class] != DBL_MAX){
						Canvas->cellsA[i][j].attractiveness +=
						/**/ F_CLASS_PLACEMENT_SCORE (SqDistsToClasses[class], class);
					}
				}
				Canvas->cellsA[i][j].attractiveness = Canvas->cellsA[i][j].attractiveness*(1000/Normalizer);
			}
		}
	}
	SORT_CELL_INDICES (Canvas);
	free (SqDistsToClasses);
}


// hyp: if Array is of length l, 0 <= pos1 < l && 0 <= pos2 < l
// swap Tab[pos1] and Tab[pos2]
void SWAP (int* IntTab, double* DoubleTab, int pos1, int pos2){

	int TempInt = IntTab[pos1];
	IntTab[pos1] = IntTab[pos2];
	IntTab[pos2] = TempInt;

	double TempDouble = DoubleTab[pos1];
	DoubleTab[pos1] = DoubleTab[pos2];
	DoubleTab[pos2] = TempDouble;
}


// hyp: start, pivot and end are valid indices for ElemIndices and ElemScores
// swaps elements in ElemIndices[start..end] and ElemScores[start..end] identically then
// returns an index PivotPlace such that after the procedure the following are true:
// - ElemScores[PivotPlace] has ElemScores[pivot]'s initial value
// - for all i < PivotPlace, ElemScores[i] <= ElemScores[PivotPlace]
// - for all i > PivotPlace, ElemScores[i] > ElemScores[PivotPlace]
int QS_PARTITION_CUT (int* ElemIndices, double* ElemScores, int start, int end, int pivot){

	SWAP (ElemIndices, ElemScores, pivot, end);
	int PivotPlace = start;
	for (int pos = start; pos < end; pos++){
		if (ElemScores[pos] >= ElemScores[end]){
			SWAP (ElemIndices, ElemScores, pos, PivotPlace);
			PivotPlace++;
		}
	}
	SWAP (ElemIndices, ElemScores, PivotPlace, end);
	return PivotPlace;
}


// hyp: start and end are valid indices for ElemIndices and ElemScores
// sorts ElemScores[start..end] in decreasing order using quicksort,
// also sorting ElemScores following the same permutation 
void QS_SORT_ELEMENTS_BY_SCORE (int* ElemIndices, double* ElemScores, int start, int end){

	if (start < end){
		int pivot = (rand () % (end-start+1)) + start;
		int PivotPlace = QS_PARTITION_CUT (ElemIndices, ElemScores, start, end, pivot);
		QS_SORT_ELEMENTS_BY_SCORE (ElemIndices, ElemScores, start, PivotPlace-1);
		QS_SORT_ELEMENTS_BY_SCORE (ElemIndices, ElemScores, PivotPlace+1, end);
	}
}