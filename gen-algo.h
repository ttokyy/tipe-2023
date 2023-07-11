#ifndef GENALGO_H
#define GENALGO_H

#include "canvas.h"
#include "dispo.h"
#include "terrain.h"
#include "display.h"


struct individual_s {
	disposition dispo;

	double potential_exploitation_ratio;

	double density_score;
	double attractiveness_score;
	double entropy_score;
	double cost_penalty;

	double total_score;
};

struct vpopulation_s {
	int num_individuals;
	struct individual_s* individualsA;
	int* sorted_individual_indices_score;
};


typedef struct individual_s individual;
typedef struct vpopulation_s* population;


extern const double cOptimalDensityToMaxDensityRatio;

extern const double cScoreFactorWeights[2];

extern const double cMutationHomeDisplacementRadius; //add: pixels

extern SDL_Renderer* RendererGENALGO;

// double F_CELL_ATTRACTIVENESS_EXPLOITATION (canvas Canvas, disposition Dispo);

// elementary score functions
double F_DENSITY_SCORE_FROM_UNIT (unit Unit, double MaxDensity);
double F_ATTRACTIVENESS_SCORE_FROM_UNIT_CELL (unit Unit, cell Cell);
double F_ENTROPY_SCORE (unit Unit);

// generating and evaluating individuals
void INITIALIZE_INDIVIDUAL (individual* pIndiv, canvas Canvas, int NumHomes);
void CALCULATE_SCORES_OF_INDIVIDUAL (individual* pIndiv, canvas Canvas); // invert
/**/ // argument order

// creating populations and ensuring correctness
population CREATE_POPULATION (canvas Canvas, int PopSize);
void SORT_INDIVIDUAL_INDICES (population Popl);

// genetic processes
individual* MUTATE_INDIVIDUAL (canvas Canvas, individual Indiv, double ProportionMutatedUnits);
individual* CROSS_INDIVIDUALS (canvas Canvas, individual Parent1, individual Parent2,
/**/ double MaxProportionOfFirst, double StdDeviation);
void RENEW_POPULATION (canvas Canvas, population Popl, double KeepProportion,
/**/ double MutateProportion, double CrossProportion);

/* population GENERATE_RANDOM_POPULATION (canvas Canvas, int PopSize, int NumHomes);
population SELECT_FITTEST_INDIVIDUALS (population Popl);
individual* MUTATE_INDIVIDUAL (canvas Canvas, individual Indiv, double ProportionMutatedUnits); */

// void ITERATE_POPULATION_RENEWAL (canvas Canvas, population Popl, double ConservationProportion);



void FREE_INDIVIDUAL (individual* pIndiv);
// individual CROSSOVER
// individual MUTATE


/* popu InitializePopulation (canvas s, int pop_size, int nb_to_place, float param_min, float param_max);

double PlacementScoreRatio (canvas canvas, dispo dispo, int numPlaced);
double PerCellDensityScore (double density);
double TotalDensityScore (canvas canvas, dispo dispo);

popu IterateGeneticAlgorithm (SDL_Renderer* renderer, canvas c, popu pop, int pop_size, int num_placed); */

#endif