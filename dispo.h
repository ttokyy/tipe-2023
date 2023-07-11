#ifndef DISPO_H
#define DISPO_H

#include "canvas.h"



// ----- STRUCTURE DEFINITIONS ----- //

struct unit_s {
	int num_homes;
	double local_density;
	double local_entropy;
};

struct vdisposition_s {
	int width_units;
	int height_units;

	struct unit_s** unitsA;
	int* sorted_unit_indices_density;

	int num_inhabitants;
	int max_possible_inhabitants;

	double max_possible_density;
	double entropy;
};

typedef struct unit_s unit;
typedef struct vdisposition_s* disposition;


// ----- GLOBAL CONSTANTS ----- //

extern const double cDensityToleranceRadius;
extern const double cEntropyToleranceRadius;
extern const double cHomeAttributionInflation;


// ----- FUNCTIONS ----- //

// creating and handling dispositions
disposition CREATE_CANVAS_DISPOSITION (canvas Canvas);
disposition COPY_DISPOSITION (canvas Canvas, disposition Dispo);

// ensuring correctness
bool IS_DISPOSITION_HOME_ASSIGNMENT_COHERENT (disposition Dispo);
void SORT_UNIT_INDICES (canvas Canvas, disposition Dispo);

// manipulating and placing homes in dispositions
bool TRANSFER_HOMES (canvas Canvas, disposition Dispo, int Amount, int i1, int j1, int i2, int j2);
void INITIALIZE_DISPOSITION_FROM_CELLS_1 (canvas Canvas, disposition Dispo, int NumHomes,
/**/ double HomeAttributionInflation);
void INITIALIZE_DISPOSITION_FROM_CELLS_2 (canvas Canvas, disposition Dispo, int NumHomes,
/**/ double HomeAttributionInflation);
void INITIALIZE_DISPOSITION_BLINDLY (canvas Canvas, disposition Dispo, int NumHomes);

// calculation and attribution functions
double F_DENSITY_CONTRIBUTION (double SqDist, int NumHomes);
double F_HOME_ATTRIBUTION_PROPORTION (double Attractiveness, double HighestAttractiveness,
/**/ disposition Dispo, double HomeAttributionInflation);
double F_ENTROPY_CONTRIBUTION (double Ratio);

// calculating disposition attributes
void CALCULATE_LOCAL_DENSITIES (canvas Canvas, disposition Dispo);
void CALCULATE_LOCAL_ENTROPIES (canvas Canvas, disposition Dispo);
void CALCULATE_ENTROPY (canvas Canvas, disposition Dispo);



// unused
double POTENTIAL_EXPLOITATION_RATIO (canvas Canvas, disposition Dispo); // OR : call this "ATTRACTIVENESS EXPLOITATION RATIO" instead ?




void FREE_DISPOSITION (disposition Dispo);
#endif