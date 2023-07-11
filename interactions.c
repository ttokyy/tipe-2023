#include "interactions.h"

// renders an empty black screen
void USER_CLEAR_SCREEN (SDL_Renderer* Renderer){

	SDL_SetRenderDrawColor (Renderer, 0, 0, 0, 0);
	SDL_RenderClear (Renderer);
	SDL_RenderPresent (Renderer);
}


// assigns to *pServiceClass the previous service class ID and readies *pRed, *pGreen and
// *pBlue for drawing in the new class's display color
void USER_CYCLE_SERVICE_CLASS_PREV (SDL_Renderer* Renderer, int* pServiceClass, int* pRed, int* pBlue, int* pGreen){

	*pServiceClass = (*pServiceClass - 1) % cNumServiceClasses;
	if (*pServiceClass < 0)
		*pServiceClass += cNumServiceClasses;
	ASSIGN_COLOR_OF_SERVICE_CLASS (*pServiceClass, pRed, pGreen, pBlue);
	SDL_SetRenderDrawColor (Renderer, *pRed, *pGreen, *pBlue, 255);
}


// assigns to *pServiceClass the next service class ID and readies *pRed, *pGreen and
// *pBlue for drawing in the new class's display color
void USER_CYCLE_SERVICE_CLASS_NEXT (SDL_Renderer* Renderer, int* pServiceClass, int* pRed, int* pBlue, int* pGreen){

	*pServiceClass = (*pServiceClass + 1) % cNumServiceClasses;
	ASSIGN_COLOR_OF_SERVICE_CLASS (*pServiceClass, pRed, pGreen, pBlue);
	SDL_SetRenderDrawColor (Renderer, *pRed, *pGreen, *pBlue, 255);
}


// fills each blank cell of Canvas hovered over by the user's cursor in ServiceClass's
// dispay color, also updating Canvas->service_border_pixels by adding midpoints of all
// newly filled cells
void USER_DRAW_SERVICE (SDL_Renderer* Renderer, int* pRed, int* pGreen, int* pBlue,
/**/ int* pMouseX, int* pMouseY, canvas Canvas, int* pFill_i, int* pFill_j, int ServiceClass){

	SDL_GetMouseState (pMouseX, pMouseY);
	ASSIGN_CELL_OF_PIXEL (pFill_i, pFill_j, Canvas, *pMouseX, *pMouseY);

	ASSIGN_COLOR_OF_SERVICE_CLASS (ServiceClass, pRed, pGreen, pBlue);
	SDL_SetRenderDrawColor (Renderer, *pRed, *pGreen, *pBlue, 255);

	if (Canvas->cellsA[*pFill_i][*pFill_j].service_presence_class == -1){

		Canvas->cellsA[*pFill_i][*pFill_j].service_presence_class = ServiceClass;
		SDL_RenderFillRect (Renderer, &Canvas->cellsA[*pFill_i][*pFill_j].fill_square); //will this work?
		SDL_RenderPresent (Renderer);

		int X, Y;
		ASSIGN_MIDPOINT_OF_CELL (&X, &Y, Canvas, *pFill_i, *pFill_j);
		l_insert (&Canvas->service_border_pixelsL, X, Y, ServiceClass);
	}
}


// hyp: Canvas' service whose borders surround the user's current cursor position has
// class ServiceClass
// fills the cells surrounded by the aforementioned service
void USER_FILL_SERVICE (SDL_Renderer* Renderer, int* pMouseX, int* pMouseY,
/**/ canvas Canvas, int* pFill_i, int* pFill_j, int ServiceClass){

	SDL_GetMouseState (pMouseX, pMouseY);
	ASSIGN_CELL_OF_PIXEL (pFill_i, pFill_j, Canvas, *pMouseX, *pMouseY);
	SDL_FloodFillService (Renderer, Canvas, ServiceClass, *pFill_i, *pFill_j);
	SDL_RenderPresent (Renderer);
}


// rerenders services which have been drawn on Canvas
void USER_RERENDER_SERVICES (SDL_Renderer* Renderer, canvas Canvas){

	SDL_RenderExistingServices (Renderer, Canvas);
}

// calculates then displays the attractivenesses of Canvas' cells
void USER_CALCULATE_AND_DISPLAY_ATTRACTIVENESSES (SDL_Renderer* Renderer, canvas Canvas){

	CALCULATE_ATTRACTIVENESSES (Canvas);
	SDL_DisplayCellAttractivenesses (Renderer, Canvas);
}


// hyp: Canvas' cell attractivenesses have been previously calculated
// displays the attractivenesses of Canvas' cells
void USER_DISPLAY_ATTRACTIVENESSES (SDL_Renderer* Renderer, canvas Canvas){

	SDL_DisplayCellAttractivenesses (Renderer, Canvas);
}


// creates and initializes a disposition on Canvas set to contain NumHomes homes using
// INITIALIZE_DISPOSITION_FROM_CELLS, then displays it
void USER_INITIALIZE_DISPOSITION_AND_DISPLAY_HOMES (SDL_Renderer* Renderer, canvas Canvas,
/**/ disposition Dispo){

	int NumHomes;
	printf ("Enter number of homes to place for test disposition :\n");
	scanf ("%d", &NumHomes);

	double HomeAttributionInflation;
	printf ("Enter inflation parameter for home attribution :\n");
	scanf ("%lf", &HomeAttributionInflation);

	INITIALIZE_DISPOSITION_FROM_CELLS_2 (Canvas, Dispo, NumHomes, HomeAttributionInflation);
	SDL_RenderExistingHomes (Renderer, Canvas, Dispo);
}


// hyp: TestDispo has been initialized
//
void USER_CALCULATE_AND_DISPLAY_DENSITIES (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo){

	CALCULATE_LOCAL_DENSITIES (Canvas, Dispo);
	SDL_DisplayUnitDensities (Renderer, Canvas, Dispo);
	SORT_UNIT_INDICES (Canvas, Dispo);
}


void USER_INITIALIZE_AND_SORT_POPULATION (canvas Canvas, population* pPopl){

	int NumHomes;
	printf ("Enter number of homes to place for population's individuals :\n");
	scanf ("%d", &NumHomes);

	int PopSize;
	printf ("Enter number of individuals to generate :\n");
	scanf ("%d", &PopSize);

	*pPopl = CREATE_POPULATION (Canvas, PopSize);
	for (int index = 0; index < PopSize; index++){
		INITIALIZE_INDIVIDUAL (((*pPopl)->individualsA)+index, Canvas, NumHomes);
		printf ("%f\n", (*pPopl)->individualsA[index].total_score);
	}
	SORT_INDIVIDUAL_INDICES (*pPopl);
}


// void USER_RENEW_POPULATION (canvas Canvas, population Popl){
// 	double ConservationProportion;
// 	printf ("Enter proportion of fittest individuals to keep and mutate :\n");
// 	scanf ("%lf", &ConservationProportion);

// 	ITERATE_POPULATION_RENEWAL (Canvas, Popl, ConservationProportion);
// }


void USER_DISPLAY_POPULATION_INDIVIDUAL_DESC_SCORE (SDL_Renderer* Renderer, canvas Canvas,
/**/ population Popl){

	int IndivIndex;
	printf ("Enter index of individual to display (between 0 and %d) :\n", Popl->num_individuals-1);
	scanf ("%d", &IndivIndex);

	USER_CLEAR_SCREEN (Renderer);
	USER_RERENDER_SERVICES (Renderer, Canvas);
	SDL_RenderExistingHomes (Renderer, Canvas,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[IndivIndex]].dispo);

	printf ("\n---------\nAttractiveness : %f\nDensity : %f\nEntropy : %f (entropy is %f)\nCost : %f\nTotal : %f\n",
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[IndivIndex]].attractiveness_score,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[IndivIndex]].density_score,
		Popl->individualsA[Popl->sorted_individual_indices_score[IndivIndex]].entropy_score,
		Popl->individualsA[Popl->sorted_individual_indices_score[IndivIndex]].dispo->entropy,
		Popl->individualsA[Popl->sorted_individual_indices_score[IndivIndex]].cost_penalty,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[IndivIndex]].total_score);
}


void USER_INITIALIZE_AND_DISPLAY_INIDIVIDUAL (canvas Canvas, disposition Dispo, individual* pIndiv){

	pIndiv->dispo = Dispo;
	pIndiv->total_score = -DBL_MAX;
	CALCULATE_SCORES_OF_INDIVIDUAL (pIndiv, Canvas);
	printf ("Attractiveness : %f || Density : %f || Total : %f \n",
	/**/ pIndiv->attractiveness_score, pIndiv->density_score, pIndiv->total_score);
}

void USER_INITIALIZE_AND_DISPLAY_INDIVIDUAL_AND_DISPO (SDL_Renderer* Renderer,
	canvas Canvas, individual* pIndiv){

	pIndiv->dispo = CREATE_CANVAS_DISPOSITION (Canvas);

	int NumHomes;
	printf ("Enter number of homes to place for test disposition :\n");
	scanf ("%d", &NumHomes);

	double HomeAttributionInflation;
	printf ("Enter inflation parameter for home attribution :\n");
	scanf ("%lf", &HomeAttributionInflation);

	INITIALIZE_DISPOSITION_FROM_CELLS_2 (Canvas, pIndiv->dispo, NumHomes, HomeAttributionInflation);
	SDL_RenderExistingHomes (Renderer, Canvas, pIndiv->dispo);

	pIndiv->total_score = -DBL_MAX;
	CALCULATE_SCORES_OF_INDIVIDUAL (pIndiv, Canvas);
}


void USER_MUTATE_AND_DISPLAY_INDIVIDUAL (SDL_Renderer* Renderer, canvas Canvas,
/**/ individual* pIndiv){

	double MutationProportion;
	printf ("Enter proportion of units to be mutated for individual :\n");
	scanf ("%lf", &MutationProportion);

	individual* pNewIndiv = MUTATE_INDIVIDUAL (Canvas, *pIndiv, MutationProportion);
	*pIndiv = *pNewIndiv;
	USER_CLEAR_SCREEN (Renderer);
	USER_RERENDER_SERVICES (Renderer, Canvas);
	SDL_RenderExistingHomes (Renderer, Canvas, pIndiv->dispo);
	CALCULATE_LOCAL_DENSITIES (Canvas, pIndiv->dispo);
	SORT_UNIT_INDICES (Canvas, pIndiv->dispo);
}


void USER_CROSS_AND_DISPLAY_INDIVIDUALS (SDL_Renderer* Renderer, canvas Canvas,
/**/ individual* pIndiv1, individual* pIndiv2){

	double MaxProportionOfFirst;
	double StdDeviation;
	printf ("Enter maximum proportion of first :\n");
	scanf ("%lf", &MaxProportionOfFirst);
	printf ("Enter standard deviation (ideally > (1/2)*MaxProportionOfFirst) :\n");
	scanf ("%lf", &StdDeviation);
	printf ("ok, %lf and %lf\n", MaxProportionOfFirst, StdDeviation);

	individual* pNewIndiv = CROSS_INDIVIDUALS (Canvas, *pIndiv1, *pIndiv2,
	/**/ MaxProportionOfFirst, StdDeviation);

	*pIndiv1 = *pNewIndiv;
	USER_CLEAR_SCREEN (Renderer);
	USER_RERENDER_SERVICES (Renderer, Canvas);
	SDL_RenderExistingHomes (Renderer, Canvas, pIndiv1->dispo);
	CALCULATE_LOCAL_DENSITIES (Canvas, pIndiv1->dispo);
	SORT_UNIT_INDICES (Canvas, pIndiv1->dispo);
}

// void USER_LAUNCH_GENETIC_ALGORITHM (SDL_Renderer* Renderer, canvas Canvas){

// 	int NumHomes, GenerationSize, NumRenewals;
// 	printf ("Enter number of homes to place on canvas :\n");
// 	scanf ("%d", &NumHomes);
// 	printf ("Enter number of individuals in population :\n");
// 	scanf ("%d", &GenerationSize);
// 	printf ("Enter the number of generations :\n");
// 	scanf ("%d", &NumRenewals);

// 	population Popl = GENERATE_RANDOM_POPULATION (Canvas, GenerationSize, NumHomes);

// 	printf("a\n");
// 	USER_CLEAR_SCREEN (Renderer);
// 	USER_RERENDER_SERVICES (Renderer, Canvas);
// 	SDL_RenderExistingHomes (Renderer, Canvas,
// 	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].dispo);

// 	printf ("Best individual stats\n---------------------\nAttractiveness : %f || Density : %f || Total : %f \n\n",
// 	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].attractiveness_score,
// 	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].density_score,
// 	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].total_score);

// 	printf ("b\n");
// 	for (int generation = 0; generation < NumRenewals; generation++){

// 		for (int individual = 0; individual < GenerationSize; individual++){
// 			assert (IS_DISPOSITION_HOME_ASSIGNMENT_COHERENT
// 			/**/ (Popl->individualsA[individual].dispo));
// 		}
// 		ITERATE_POPULATION_RENEWAL (Canvas, Popl, 0.6);

// 		USER_CLEAR_SCREEN (Renderer);
// 		USER_RERENDER_SERVICES (Renderer, Canvas);
// 		SDL_RenderExistingHomes (Renderer, Canvas,
// 		/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].dispo);

// 		printf ("Best individual stats\n---------------------\nAttractiveness : %f || Density : %f || Total : %f \n\n",
// 		/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].attractiveness_score,
// 		/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].density_score,
// 		/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].total_score);

// 	}
// 	printf ("\n\nDone.\n");
// }


void USER_CALCULATE_AND_DISPLAY_ENTROPIES (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo){

	CALCULATE_LOCAL_ENTROPIES (Canvas, Dispo);
	SDL_DisplayUnitEntropies (Renderer, Canvas, Dispo);
}

void USER_COMPUTE_AND_DISPLAY_HEIGHTS (SDL_Renderer* Renderer, canvas Canvas){

	double Amplitude;
	int Frequency;
	int NumOctaves;
	double Lacunarity;
	double Persistence;
	double Exponentiation;
	printf ("Enter following parameters in order, seperated by semicolons : amplitude, frequency, number of octaves, lacunarity, persistence, expontentiation.\n");
	scanf ("%lf ; %d ; %d ; %lf ; %lf ; %lf", &Amplitude, &Frequency, &NumOctaves, &Lacunarity,
	/**/ &Persistence, &Exponentiation);
	printf ("%f ; %d ; %d ; %f ; %f ; %f\n", Amplitude, Frequency, NumOctaves, Lacunarity,
	/**/ Persistence, Exponentiation);

	GENERATE_TERRAIN_HEIGHTMAP (Canvas, Amplitude, Frequency, NumOctaves, Lacunarity,
	/**/ Persistence, Exponentiation);

	SDL_DisplayTerrainHeights (Renderer, Canvas);
}

void USER_DISPLAY_TERRAIN (SDL_Renderer* Renderer, canvas Canvas){

	SDL_RenderTerrain (Renderer, Canvas);
}





void INITIALIZE_AND_EVOLVE_POPULATION (SDL_Renderer* Renderer, canvas Canvas){

	int NumHomes;
	int NumIndividuals;
	int NumGenerations;

	double KeepProportion;
	double MutateProportion;
	double CrossProportion;

	printf ("Enter : NumHomes, NumIndividuals, NumGenerations, KeepProportion, MutateProportion, CrossProportion.\n");
	scanf ("%d ; %d ; %d ; %lf ; %lf ; %lf", &NumHomes, &NumIndividuals, &NumGenerations,
	/**/ &KeepProportion, &MutateProportion, &CrossProportion);

	// printf ("a\n");

	// initialize first generation
	population Popl = CREATE_POPULATION (Canvas, NumIndividuals);
	for (int index = 0; index < Popl->num_individuals; index++){

		INITIALIZE_INDIVIDUAL (&Popl->individualsA[index], Canvas, NumHomes);
	}
	SORT_INDIVIDUAL_INDICES (Popl);

	// printf ("b\n");

	USER_CLEAR_SCREEN (Renderer);
	USER_RERENDER_SERVICES (Renderer, Canvas);
	// USER_DISPLAY_TERRAIN (Renderer, Canvas);
	SDL_RenderExistingHomes (Renderer, Canvas,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].dispo);

	// printf ("c\n");

	printf ("INIT_POP:\n----------\n");
	for (int index = 0; index < NumIndividuals; index++){
		printf ("IND#%d : Attractiveness: %f | Density: %f | Entropy: %f | Cost penalty: %f || Total: %f\n",
		/**/ index,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].attractiveness_score,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].density_score,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].entropy_score,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].cost_penalty,
	/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].total_score);
	}
	printf ("\n");

	// iterate renewal for NumGenerations generations
	for (int generation = 0; generation < NumGenerations; generation++){

		RENEW_POPULATION (Canvas, Popl, KeepProportion, MutateProportion, CrossProportion);

		USER_CLEAR_SCREEN (Renderer);
		USER_RERENDER_SERVICES (Renderer, Canvas);
		// USER_DISPLAY_TERRAIN (Renderer, Canvas);
		SDL_RenderExistingHomes (Renderer, Canvas,
		/**/ Popl->individualsA[Popl->sorted_individual_indices_score[0]].dispo);

		printf ("POP %d:\n----------\n", generation+1);
		for (int index = 0; index < NumIndividuals; index++){
			printf ("IND#%d : Attractiveness: %f | Density: %f | Entropy: %f | Cost penalty: %f || Total: %f\n",
			/**/ index,
			/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].attractiveness_score,
			/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].density_score,
			/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].entropy_score,
			/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].cost_penalty,
			/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].total_score);
		}
		printf ("\n");
	}
}



void USER_INITIALIZE_POPULATION (canvas Canvas, population* pPopl,
/**/ double* pKeepProportion,
/**/ double* pMutateProportion){
	
	int NumHomes;
	int NumIndividuals;

	printf ("Enter : NumHomes, NumIndividuals, KeepProportion, MutateProportion.\n");
	scanf ("%d ; %d ; %lf ; %lf", &NumHomes, &NumIndividuals,
	/**/ pKeepProportion, pMutateProportion);

	*pPopl = CREATE_POPULATION (Canvas, NumIndividuals);
	for (int index = 0; index < (*pPopl)->num_individuals; index++){

		INITIALIZE_INDIVIDUAL (&(*pPopl)->individualsA[index], Canvas, NumHomes);
	}
	SORT_INDIVIDUAL_INDICES (*pPopl);

	printf ("\n----------\n");
	for (int index = 0; index < NumIndividuals; index++){
		printf ("IND#%d : Attractiveness: %f | Density: %f | Entropy: %f | Cost penalty: %f || Total: %f\n",
		/**/ index,
		/**/ (*pPopl)->individualsA[(*pPopl)->sorted_individual_indices_score[index]].attractiveness_score,
		/**/ (*pPopl)->individualsA[(*pPopl)->sorted_individual_indices_score[index]].density_score,
		/**/ (*pPopl)->individualsA[(*pPopl)->sorted_individual_indices_score[index]].entropy_score,
		/**/ (*pPopl)->individualsA[(*pPopl)->sorted_individual_indices_score[index]].cost_penalty,
		/**/ (*pPopl)->individualsA[(*pPopl)->sorted_individual_indices_score[index]].total_score);
	}
	printf ("\n");
}

void USER_GENETIC_ALGORITHM_STEP (canvas Canvas, population Popl,
/**/ double KeepProportion, double MutateProportion){

	RENEW_POPULATION (Canvas, Popl, KeepProportion, MutateProportion, 0);
	int NumIndividuals = Popl->num_individuals;

	printf ("\n----------\n");
	for (int index = 0; index < NumIndividuals; index++){
		printf ("IND#%d : Attractiveness: %f | Density: %f | Entropy: %f | Cost penalty: %f || Total: %f\n",
		/**/ index,
		/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].attractiveness_score,
		/**/ Popl->individualsA[Popl->sorted_individual_indices_score[index]].density_score,
		/**/ (Popl)->individualsA[Popl->sorted_individual_indices_score[index]].entropy_score,
		/**/ (Popl)->individualsA[Popl->sorted_individual_indices_score[index]].cost_penalty,
		/**/ (Popl)->individualsA[Popl->sorted_individual_indices_score[index]].total_score);
	}
	printf ("\n");
}


void USER_GENETIC_ALGORITHM (SDL_Renderer* Renderer, canvas Canvas){

	INITIALIZE_AND_EVOLVE_POPULATION (Renderer, Canvas);
}