#include "dispo.h"


const double cDensityToleranceRadius = 100;
const double cEntropyToleranceRadius = 50;

const double cHomeAttributionInflation = 0.6; // the smaller, the more tolerant attribution
											  // is with regards to attractiveness


// returns an empty disposition (i.e. with all attributes initialized to their default
// value) that is valid for Canvas (i.e. whose dimensions in units are those of
// Canvas in cells)
disposition CREATE_CANVAS_DISPOSITION (canvas Canvas){

	disposition Dispo = (disposition) malloc (sizeof(struct vdisposition_s));
	Dispo->width_units = Canvas->width_cells;
	Dispo->height_units = Canvas->height_cells;

	Dispo->num_inhabitants = 0;
	Dispo->max_possible_inhabitants = Dispo->width_units*Dispo->height_units*cCellHomeLimit;

	Dispo->unitsA = (unit**) malloc (Dispo->height_units*sizeof(unit*));

	Dispo->sorted_unit_indices_density = (int*) malloc
	/**/ (Dispo->width_units*Dispo->height_units*sizeof(int));

	// initialize units
	for (int i = 0; i < Dispo->height_units; i++){

		Dispo->unitsA[i] = (unit*) malloc (Dispo->width_units*sizeof(unit));

		for (int j = 0; j < Dispo->width_units; j++){

			Dispo->unitsA[i][j].num_homes = 0;
			Dispo->unitsA[i][j].local_density = 0;
			Dispo->sorted_unit_indices_density[LIN_COEFF (Canvas, i, j)] =
			/**/ LIN_COEFF (Canvas, i, j);
		}

	}
	Dispo->entropy = 0;

	// calculate maximum possible density
	Dispo->max_possible_density = 0;

	int X_min = -(int)cDensityToleranceRadius; int X_max = (int)cDensityToleranceRadius;
	int Y_min = -(int)cDensityToleranceRadius; int Y_max = (int)cDensityToleranceRadius;
	int InscribingSquareSideCells = (int)ceil ((float)(X_max-X_min)/(float)cCellSizePixels);

	for (int row = 0; row <= InscribingSquareSideCells; row++){
		for (int column = 0; column <= InscribingSquareSideCells; column++){

			int CurrentX = X_min + column*cCellSizePixels;
			int CurrentY = Y_min + row*cCellSizePixels;
			double SqDist = CurrentX*CurrentX + CurrentY*CurrentY;

			if (SqDist <= cDensityToleranceRadius*cDensityToleranceRadius){
					Dispo->max_possible_density += F_DENSITY_CONTRIBUTION
					/**/ (SqDist, cCellHomeLimit);
			}
		}
	}
	return Dispo;
}



void FREE_DISPOSITION (disposition Dispo){

	/* printf ("freeing dispo\n");
	free (Dispo->unitsA);
	free (Dispo->sorted_unit_indices_density); */
}


// hyp: Dispo is a disposition on Canvas
// returns a copy of Dispo
disposition COPY_DISPOSITION (canvas Canvas, disposition Dispo){

	disposition DispoCopy = (disposition) malloc (sizeof(struct vdisposition_s));
	DispoCopy->width_units = Dispo->width_units;
	DispoCopy->height_units = Dispo->height_units;

	DispoCopy->num_inhabitants = Dispo->num_inhabitants;
	DispoCopy->max_possible_inhabitants = Dispo->max_possible_inhabitants;
	DispoCopy->max_possible_density = Dispo->max_possible_density;

	DispoCopy->unitsA = (unit**) malloc (DispoCopy->height_units*sizeof(unit*));

	DispoCopy->sorted_unit_indices_density = (int*) malloc
	/**/ (DispoCopy->height_units*DispoCopy->width_units*sizeof(int));

	for (int i = 0; i < DispoCopy->height_units; i++){

		DispoCopy->unitsA[i] = (unit*) malloc (DispoCopy->width_units*sizeof(unit));

		for (int j = 0; j < DispoCopy->width_units; j++){

			DispoCopy->unitsA[i][j].num_homes = Dispo->unitsA[i][j].num_homes;
			DispoCopy->unitsA[i][j].local_density = Dispo->unitsA[i][j].local_density;
			DispoCopy->unitsA[i][j].local_entropy = Dispo->unitsA[i][j].local_entropy;
			DispoCopy->sorted_unit_indices_density[LIN_COEFF (Canvas, i, j)] =
			/**/ Dispo->sorted_unit_indices_density[LIN_COEFF (Canvas, i, j)];
		}
	}
	return DispoCopy;
}


// returns whether or not both statements are simultaneously true:
// - all of Dispo's cells contain between 0 and cCellHomeLimit houses
// - summing the amount of homes per cell across all cells yields Dispo->num_inhabitants
bool IS_DISPOSITION_HOME_ASSIGNMENT_COHERENT (disposition Dispo){

	int NumHomes = 0;
	for (int i = 0; i < Dispo->height_units; i++){
		for (int j = 0; j < Dispo->width_units; j++){
			if (Dispo->unitsA[i][j].num_homes < 0) return false;
			NumHomes += Dispo->unitsA[i][j].num_homes;
		}
	}
	return (NumHomes == Dispo->num_inhabitants);
}


// sorts Dispo->sorted_unit_indices_density such that the sequence
// (Dispo->unitsA[ik][jk].local_density), where (ik,jk) is the delinearized coefficient
// obtained from k, increases in k
void SORT_UNIT_INDICES (canvas Canvas, disposition Dispo){

	int NumUnits = Dispo->height_units*Dispo->width_units;
	int* UnitIndices = Dispo->sorted_unit_indices_density;
	double* UnitDensities = (double*) malloc (NumUnits*sizeof(double));

	for (int index = 0; index < NumUnits; index++){

		int ci, cj;
		ASSIGN_DELIN_INDEX (&ci, &cj, Canvas, UnitIndices[index]);
		UnitDensities[index] = Dispo->unitsA[ci][cj].local_density;
	}

	QS_SORT_ELEMENTS_BY_SCORE (UnitIndices, UnitDensities, 0, NumUnits-1);

	/* printf ("freeing unitdensities\n");
	free (UnitDensities); */
}


// attempts to move Amount homes between Dispo's units of indices (i1,j1) and (i2,j2)
// while ensuring that their numbers of homes remains between 0 and cCellHomeLimit
// returns true if and only if a non-zero number of homes were actually moved
bool TRANSFER_HOMES (canvas Canvas, disposition Dispo, int Amount, int i1, int j1, int i2, int j2){

	// printf (" into %d\n", Dispo->unitsA[i2][j2].num_homes);
	Amount = min (min (Amount, Dispo->unitsA[i1][j1].num_homes), cCellHomeLimit - Dispo->unitsA[i2][j2].num_homes);
	if (Amount > 0 && Canvas->cellsA[i2][j2].service_presence_class == -1){

		Dispo->unitsA[i1][j1].num_homes -= Amount;
		Dispo->unitsA[i2][j2].num_homes += Amount;
		return true;
	}
	else return false;
}


// hyp: Dispo is a disposition on Canvas && CALCULATE_ATTRACTIVENESSES (Canvas) has
// been called
// distributes NumHomes homes to Dispo's units according to a random process in which
// each unit's expected final amount of homes increases with the corresponding cell's
// attractiveness value in Canvas
void INITIALIZE_DISPOSITION_FROM_CELLS_1 (canvas Canvas, disposition Dispo, int NumHomes,
double HomeAttributionInflation){ // METHOD 1

	assert (NumHomes <= Dispo->max_possible_inhabitants);
	Dispo->num_inhabitants = NumHomes;
	// printf ("%d/%d\n", Dispo->num_inhabitants, Dispo->max_possible_inhabitants);

	// reset disposition's housing
	for (int i = 0; i < Canvas->height_cells; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			Dispo->unitsA[i][j].num_homes = 0;
		}
	}
	// sort cell indices by descending attractivenesses and extract highest attractiveness
	SORT_CELL_INDICES (Canvas);

	int BestCell_i, BestCell_j;
	ASSIGN_DELIN_INDEX (&BestCell_i, &BestCell_j, Canvas, Canvas->sorted_cell_indices_attractiveness[0]);
	double HighestAttractiveness = Canvas->cellsA[BestCell_i][BestCell_j].attractiveness;

	int NumRemainingHomes = NumHomes;
	int pos = 0;
	int ci, cj;

	// distribute homes by prioritising high-attractiveness cells
	while (NumRemainingHomes > 0){

		ASSIGN_DELIN_INDEX (&ci, &cj, Canvas, Canvas->sorted_cell_indices_attractiveness[pos]);
		if (Canvas->cellsA[ci][cj].service_presence_class == -1){

			int NumReceivedHomes = min (min (NumRemainingHomes, cCellHomeLimit), rand () % //-Dispo->unitsA[ci][cj].num_homes), rand () %
			/**/ ((int)(ceil ((double)cCellHomeLimit)*F_HOME_ATTRIBUTION_PROPORTION
			/**/ (Canvas->cellsA[ci][cj].attractiveness, HighestAttractiveness, Dispo,
			/**/ HomeAttributionInflation))+ 1));
			Dispo->unitsA[ci][cj].num_homes += NumReceivedHomes;
			NumRemainingHomes -= NumReceivedHomes;
			// printf ("%d\n", NumReceivedHomes);
		}
		pos = (pos + 1) % (Canvas->width_cells*Canvas->height_cells);
	}
}


// hyp: Dispo is a disposition on Canvas && CALCULATE_ATTRACTIVENESSES (Canvas) has
// been called
// distributes NumHomes homes to Dispo's units according to a random process in which
// each unit's expected final amount of homes increases with the corresponding cell's
// attractiveness value in Canvas
void INITIALIZE_DISPOSITION_FROM_CELLS_2 (canvas Canvas, disposition Dispo, int NumHomes,
/**/ double HomeAttributionInflation){ // METHOD 2

	assert (NumHomes <= Dispo->max_possible_inhabitants);
	Dispo->num_inhabitants = NumHomes;
	// printf ("%d/%d\n", Dispo->num_inhabitants, Dispo->max_possible_inhabitants);

	// reset disposition's housing
	for (int i = 0; i < Canvas->height_cells; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			Dispo->unitsA[i][j].num_homes = 0;
		}
	}
	// sort cell indices by descending attractivenesses and extract highest attractiveness
	SORT_CELL_INDICES (Canvas);

	int BestCell_i, BestCell_j;
	ASSIGN_DELIN_INDEX (&BestCell_i, &BestCell_j, Canvas,
	/**/ Canvas->sorted_cell_indices_attractiveness[0]);
	double HighestAttractiveness = Canvas->cellsA[BestCell_i][BestCell_j].attractiveness;

	int NumRemainingHomes = NumHomes;
	int pos = 0;
	int ci, cj;

	// distribute homes by prioritising high-attractiveness cells
	while (NumRemainingHomes > 0){

		ASSIGN_DELIN_INDEX (&ci, &cj, Canvas,
		/**/ Canvas->sorted_cell_indices_attractiveness[pos]);
		if (Canvas->cellsA[ci][cj].service_presence_class == -1){

			double AttributionPeak = F_HOME_ATTRIBUTION_PROPORTION
			/**/ (Canvas->cellsA[ci][cj].attractiveness, HighestAttractiveness, Dispo,
			/**/ HomeAttributionInflation);

			int NumReceivedHomes = min (min (NumRemainingHomes,
			/**/ cCellHomeLimit-Dispo->unitsA[ci][cj].num_homes),
			/**/ max (1, (int)ceil((double)cCellHomeLimit* // previously, max (0, ...)
			/**/ RAND_VAR_HALF_NORMAL_DISTRIBUTION (AttributionPeak, 0.25*AttributionPeak))));

			Dispo->unitsA[ci][cj].num_homes += NumReceivedHomes;
			NumRemainingHomes -= NumReceivedHomes;
		}
		pos = (pos + 1) % (Canvas->width_cells*Canvas->height_cells);
	}
}


void INITIALIZE_DISPOSITION_BLINDLY (canvas Canvas, disposition Dispo, int NumHomes){

	assert (NumHomes <= Dispo->max_possible_inhabitants);
	Dispo->num_inhabitants = NumHomes;

	// reset disposition's housing
	for (int i = 0; i < Canvas->height_cells; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			Dispo->unitsA[i][j].num_homes = 0;
		}
	}

	int NumRemainingHomes = NumHomes;
	while (NumRemainingHomes > 0){
		
		int i = rand () % Canvas->height_cells;
		int j = rand () % Canvas->width_cells;
		
		if (Canvas->cellsA[i][j].service_presence_class == -1){
			int NumReceivedHomes = min (cCellHomeLimit - Dispo->unitsA[i][j].num_homes,
				rand () % (cCellHomeLimit+1));

			Dispo->unitsA[i][j].num_homes += NumReceivedHomes;
			NumRemainingHomes -= NumReceivedHomes;
		}
	}

}

// returns the amount of homes for which a unit with NumHomes houses accounts for
// in the density calculation of a unit SqDist pixels away
double F_DENSITY_CONTRIBUTION (double SqDist, int NumHomes){

	return ((double)NumHomes)*exp(-SqDist/10000);
}


// hyp: Dispo->num_inhabitants has been assigned the intented number of homes to place
// returns the fraction of cCellHomeLimit's homes given to a cell of attractiveness
// Attractiveness, living in a canvas of highest attractiveness HighestAttractiveness
// and depending on Dispo's intended and maximum number of houses
// (see paper for more precise description of parameter influences)
double F_HOME_ATTRIBUTION_PROPORTION (double Attractiveness, double HighestAttractiveness,
/**/ disposition Dispo, double HomeAttributionInflation){

	double v = pow(((double)Dispo->max_possible_inhabitants)/(2*(double)Dispo->num_inhabitants),
	/**/ HomeAttributionInflation)*(Attractiveness-HighestAttractiveness);
	double u = 1 + v/(15*pow(HighestAttractiveness, 0.75));
	double w = 1.03*u*exp(-((875-1000*u)/300)*((875-1000*u)/300)*((875-1000*u)/300)*((875-1000*u)/300));
	return max_double (0, w);
}


double F_ENTROPY_CONTRIBUTION (double Ratio){
	if (Ratio == 0) return 0;
	else return -Ratio*log(Ratio);
}


// hyp: Dispo is a disposition on Canvas
// calculates and assigns to each Dispo->unit[i][j] its local house density, i.e.
// the distance-weighted sum of the number of homes contained in cells within distance
// cDensityToleranceRadius of said unit
void CALCULATE_LOCAL_DENSITIES (canvas Canvas, disposition Dispo){ // METHOD 1

	for (int i = 0; i < Dispo->height_units; i++){
		for (int j = 0; j < Dispo->width_units; j++){

			if (Canvas->cellsA[i][j].service_presence_class == -1){

				Dispo->unitsA[i][j].local_density = 0;

				int UnitX, UnitY;
				ASSIGN_MIDPOINT_OF_CELL (&UnitX, &UnitY, Canvas, i, j);

				// find range of pixels that are for sure within distance cDensityToleranceRadius
				// (forming a square of sidelength 2*cDensityToleranceRadius)
				int X_min = max (UnitX - (int)cDensityToleranceRadius, 0);
				int X_max = min (UnitX + (int)cDensityToleranceRadius, cWindowDims[0]-1);
				int Y_min = max (UnitY - (int)cDensityToleranceRadius, 0);
				int Y_max = min (UnitY + (int)cDensityToleranceRadius, cWindowDims[1]-1);
				int i_min, i_max, j_min, j_max;

				// convert to unit indices
				ASSIGN_CELL_OF_PIXEL (&i_min, &j_min, Canvas, X_min, Y_min);
				ASSIGN_CELL_OF_PIXEL (&i_max, &j_max, Canvas, X_max, Y_max);

				// add up unit contributions
				for (int row = i_min; row <= i_max; row++){
					for (int column = j_min; column <= j_max; column++){

						int CurrentX, CurrentY;
						ASSIGN_MIDPOINT_OF_CELL (&CurrentX, &CurrentY, Canvas, row, column);
						double SqDist = (CurrentX-UnitX)*(CurrentX-UnitX) + (CurrentY-UnitY)*(CurrentY-UnitY);

						// count contribution iff the distance is actually less than cDensityToleranceRadius
						if (SqDist <= cDensityToleranceRadius*cDensityToleranceRadius)
							Dispo->unitsA[i][j].local_density += F_DENSITY_CONTRIBUTION
							/**/ (SqDist, Dispo->unitsA[row][column].num_homes);
					}
				}
			}
		}
	}
}


void CALCULATE_LOCAL_ENTROPIES (canvas Canvas, disposition Dispo){

	int* HomeCountOccurrences = (int*) malloc ((cCellHomeLimit+1)*sizeof(int));
	Dispo->entropy = 0;

	for (int i = 0; i < Dispo->height_units; i++){
		for (int j = 0; j < Dispo->width_units; j++){

			if (Canvas->cellsA[i][j].service_presence_class == -1){

				int NumCellsInRange = 0;
				for (int num_homes = 0; num_homes <= cCellHomeLimit; num_homes++){

					HomeCountOccurrences[num_homes] = 0;
				}

				Dispo->unitsA[i][j].local_entropy = 0;
				
				int UnitX, UnitY;
				ASSIGN_MIDPOINT_OF_CELL (&UnitX, &UnitY, Canvas, i, j);

				// find range of pixels that are for sure within distance cEntropyToleranceRadius
				// (forming a square of sidelength 2*cEntropyToleranceRadius)
				int X_min = max (UnitX - (int)cEntropyToleranceRadius, 0);
				int X_max = min (UnitX + (int)cEntropyToleranceRadius, cWindowDims[0]-1);
				int Y_min = max (UnitY - (int)cEntropyToleranceRadius, 0);
				int Y_max = min (UnitY + (int)cEntropyToleranceRadius, cWindowDims[1]-1);
				int i_min, i_max, j_min, j_max;

				// convert to unit indices
				ASSIGN_CELL_OF_PIXEL (&i_min, &j_min, Canvas, X_min, Y_min);
				ASSIGN_CELL_OF_PIXEL (&i_max, &j_max, Canvas, X_max, Y_max);

				// add up entropy contributions contributions
				for (int row = i_min; row <= i_max; row++){
					for (int column = j_min; column <= j_max; column++){

						int CurrentX, CurrentY;
						ASSIGN_MIDPOINT_OF_CELL (&CurrentX, &CurrentY, Canvas, row, column);
						double SqDist = (CurrentX-UnitX)*(CurrentX-UnitX) + (CurrentY-UnitY)*(CurrentY-UnitY);

						// count contribution iff the distance is actually less than cEntropyToleranceRadius
						if (SqDist <= cEntropyToleranceRadius*cEntropyToleranceRadius &&
						/**/ Canvas->cellsA[row][column].service_presence_class == -1){

							HomeCountOccurrences[Dispo->unitsA[row][column].num_homes]++;
							NumCellsInRange++;
						}
					}
				}

				for (int num_homes = 0; num_homes <= cCellHomeLimit; num_homes++){

					double Proportion = (double)HomeCountOccurrences[num_homes]
					/**/ /(double)NumCellsInRange;
					Dispo->unitsA[i][j].local_entropy += F_ENTROPY_CONTRIBUTION (Proportion);
				}

				Dispo->entropy += Dispo->unitsA[i][j].local_entropy;
			}
		}
	}
	/* printf ("freeing homecountoccurrences\n");
	free (HomeCountOccurrences); */
}