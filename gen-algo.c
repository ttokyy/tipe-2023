#include "gen-algo.h"

const double cOptimalDensityToMaxDensityRatio = 0.15;

const double cMutationHomeDisplacementRadius = 800;

SDL_Renderer* RendererGENALGO;


// returns the signed density score contribution of Unit, which accounts for its local
// density in comparison to the theoretical maximum possible density in a manner dictated
// by cOptimalDensityToMaxDensityRatio, and is proportional to Unit.num_homes
double F_DENSITY_SCORE_FROM_UNIT (unit Unit, double MaxDensity){

	double y = (0.96/cOptimalDensityToMaxDensityRatio)*(Unit.local_density/MaxDensity);
	return 0.25*Unit.num_homes*((1.8*exp(-(y-1)*(y-1))) - 1./(1.+exp(-y-0.5)));
}

// hyp : Unit's corresponding canvas cell is Cell
// returns the attractiveness score contribution of Unit based on Cell's attractiveness
double F_ATTRACTIVENESS_SCORE_FROM_UNIT_CELL (unit Unit, cell Cell){

	return 1.5*Unit.num_homes*(Cell.attractiveness/1000);
}


// returns the entropy score ontribution of a Unit
double F_ENTROPY_SCORE (unit Unit){

	double u = 6*Unit.local_entropy/log(cCellHomeLimit);
	return 0.5*(1.1*exp(-u*u*u/10)-0.12*(u+exp(-1))*log(u+exp(-1))-1);
}


double F_COST_PENALTY (unit Unit, cell Cell, int NumHomes){

	double CostCoeff = cTerrainCosts[TERRAIN_TYPE_OF_HEIGHT (Cell.terrain_height)];
	return -(0.001*CostCoeff)*(double)Unit.num_homes*(1-Cell.terrain_height)
	/**/ *(1-Cell.terrain_height)*0;
}


// intiializes an individual whose disposition is obtained by calling CREATE_CANVAS_
// DISPOSITION with a random HomeAttributionInflation parameter
// then, calculates said individuals's scores (therefore calculating it's disposition's
// densities and entropies) before returning it 
void INITIALIZE_INDIVIDUAL (individual* pIndiv, canvas Canvas, int NumHomes){

	pIndiv->potential_exploitation_ratio = 0;
	pIndiv->density_score = 0;
	pIndiv->attractiveness_score = 0;
	pIndiv->total_score = -DBL_MAX;

	pIndiv->dispo = CREATE_CANVAS_DISPOSITION (Canvas);

	double RandHomeAttributionInflation = 2.*((double)rand()/(double)RAND_MAX);

	INITIALIZE_DISPOSITION_BLINDLY (Canvas, pIndiv->dispo,
		/**/ NumHomes); //, RandHomeAttributionInflation);

	CALCULATE_LOCAL_DENSITIES (Canvas, pIndiv->dispo);
	CALCULATE_LOCAL_ENTROPIES (Canvas, pIndiv->dispo);
	CALCULATE_SCORES_OF_INDIVIDUAL (pIndiv, Canvas);
}


// calculates and assigns to *pIndiv its attractiveness, density and total score if the
// latter has not yet been calculated, i.e. has its initializing value -DBL_MAX
void CALCULATE_SCORES_OF_INDIVIDUAL (individual* pIndiv, canvas Canvas){

	if (pIndiv->total_score == -DBL_MAX){

		pIndiv->density_score = 0;
		pIndiv->attractiveness_score = 0;
		pIndiv->entropy_score = 0;
		pIndiv->cost_penalty = 0;

		for (int i = 0; i < pIndiv->dispo->height_units; i++){
			for (int j = 0; j < pIndiv->dispo->width_units; j++){

				if (pIndiv->dispo->unitsA[i][j].num_homes > 0){

					pIndiv->density_score += F_DENSITY_SCORE_FROM_UNIT (pIndiv->dispo->unitsA[i][j],
					/**/ pIndiv->dispo->max_possible_density);
					pIndiv->attractiveness_score += F_ATTRACTIVENESS_SCORE_FROM_UNIT_CELL
					/**/ (pIndiv->dispo->unitsA[i][j], Canvas->cellsA[i][j]);
					// printf ("%f | ", pIndiv->density_score);
					pIndiv->entropy_score += F_ENTROPY_SCORE (pIndiv->dispo->unitsA[i][j]);
					pIndiv->cost_penalty += F_COST_PENALTY (pIndiv->dispo->unitsA[i][j],
					/**/ Canvas->cellsA[i][j], pIndiv->dispo->num_inhabitants);
				}
			}
		}
		pIndiv->total_score = pIndiv->density_score + pIndiv->attractiveness_score
		/**/ + pIndiv->entropy_score + pIndiv->cost_penalty;
	}
}


// allocates memory for a population of PopSize individuals and returns the corresponding
// pointer
population CREATE_POPULATION (canvas Canvas, int PopSize){

	population Popl = (population) malloc (sizeof(struct vpopulation_s));

	Popl->num_individuals = PopSize;
	Popl->individualsA = (individual*) malloc (PopSize*sizeof(individual));
	Popl->sorted_individual_indices_score = (int*) malloc (PopSize*sizeof(int));

	for (int index = 0; index < PopSize; index++){

		Popl->sorted_individual_indices_score[index] = index;
	}

	return Popl;
}


// sorts Popl->sorted_individual_indices_score such that the sequence
// (Popl->individualsA[k]) increases in k
void SORT_INDIVIDUAL_INDICES (population Popl){

	int* IndividualIndices = Popl->sorted_individual_indices_score;
	double* IndividualScores = (double*) malloc (Popl->num_individuals*sizeof(double));

	for (int index = 0; index < Popl->num_individuals; index++){

		IndividualScores[index] = Popl->individualsA[IndividualIndices[index]].total_score;
	}

	QS_SORT_ELEMENTS_BY_SCORE (IndividualIndices, IndividualScores, 0,
	/**/Popl->num_individuals-1);

	/* printf ("freeing individualscores\n");
	free (IndividualScores); */
}

individual* MUTATE_INDIVIDUAL (canvas Canvas, individual Indiv, double ProportionMutatedUnits){

	SORT_UNIT_INDICES (Canvas, Indiv.dispo);

	individual* pMutatedIndiv = (individual*) malloc (sizeof(individual));
	pMutatedIndiv->dispo = COPY_DISPOSITION (Canvas, Indiv.dispo);

	// number of units to transfer homes from
	int NumRemainingUnits = (int) ceil (ProportionMutatedUnits*Indiv.dispo->height_units
	/**/ *Indiv.dispo->width_units);
	// printf ("%d to mutate\n (proportion: %f)", NumRemainingUnits, ProportionMutatedUnits);

	int pos = 0;
	int ci, cj;

	while (NumRemainingUnits > 0){

		ASSIGN_DELIN_INDEX (&ci, &cj, Canvas, Indiv.dispo->sorted_unit_indices_density[pos]);

		int UnitX, UnitY;
		ASSIGN_MIDPOINT_OF_CELL (&UnitX, &UnitY, Canvas, ci, cj);

		double RandDouble = ((double)rand()/(double)RAND_MAX);
		if (RandDouble <= Indiv.dispo->unitsA[ci][cj].local_density){

			int X_min = max (UnitX - cMutationHomeDisplacementRadius, 0);
			int X_max = min (UnitX + cMutationHomeDisplacementRadius, cWindowDims[0]-1);
			int Y_min = max (UnitY - cMutationHomeDisplacementRadius, 0);
			int Y_max = min (UnitY + cMutationHomeDisplacementRadius, cWindowDims[1]-1);

			int DestinationX, DestinationY;
			double SqDist = DBL_MAX;

			while (SqDist > cMutationHomeDisplacementRadius){

				DestinationX = rand () % (X_max - X_min + 1) + X_min;
				DestinationY = rand () % (Y_max - Y_min + 1) + Y_min;
				SqDist = (DestinationX-UnitX)*(DestinationX-UnitX) +
				/**/ (DestinationY-UnitY)*(DestinationY-UnitY);
			}
			int Destination_i, Destination_j;
			ASSIGN_CELL_OF_PIXEL (&Destination_i, &Destination_j, Canvas,
			/**/ DestinationX, DestinationY);

			int NumTransferredHomes = rand () % (cCellHomeLimit+1);
			if (TRANSFER_HOMES (Canvas, pMutatedIndiv->dispo, NumTransferredHomes,
			/**/ ci, cj, Destination_i, Destination_j)){
				NumRemainingUnits--;
			}
		}
		pos++;
	}
	pMutatedIndiv->total_score = -DBL_MAX;
	CALCULATE_LOCAL_DENSITIES (Canvas, pMutatedIndiv->dispo);
	CALCULATE_LOCAL_ENTROPIES (Canvas, pMutatedIndiv->dispo);
	CALCULATE_SCORES_OF_INDIVIDUAL (pMutatedIndiv, Canvas);
	return pMutatedIndiv;
}


individual* CROSS_INDIVIDUALS (canvas Canvas, individual Parent1, individual Parent2,
/**/ double MaxProportionOfFirst, double StdDeviation){

	/* int NumUnits = Canvas->height_cells*Canvas->width_cells;
	int* ShuffledUnits = (int*) malloc (NumUnits*sizeof(int));
	for (int pos = 0; pos < NumUnits; pos++){
		ShuffledUnits[pos] = pos;
	}
	for (int num_swapped = 0; num_swapped < NumSwaps; num_swapped++){

		int pos1 = rand () % (NumUnits);
		int pos2 = pos1;
		while (pos2 == pos1){
			pos2 = rand () % (NumUnits);
		}

		int TempIndex = ShuffledUnits[pos1];
		ShuffledUnits[pos1] = ShuffledUnits[pos2];
		ShuffledUnits[pos2] = ShuffledUnits[pos1];
	}

			int end_index = (int)ceil(ProportionUnitsFromFirst*NumUnits); */

	individual* pChildIndiv = (individual*) malloc (sizeof(individual));
	pChildIndiv->total_score = -DBL_MAX;

	pChildIndiv->dispo = CREATE_CANVAS_DISPOSITION (Canvas);
	pChildIndiv->dispo->num_inhabitants = Parent1.dispo->num_inhabitants;

	int NumRemainingHomes = Parent1.dispo->num_inhabitants;

	for (int index = 0; index < Canvas->height_cells*Canvas->width_cells &&
	/**/ NumRemainingHomes > 0; index++){

		int ci, cj;
		ASSIGN_DELIN_INDEX (&ci, &cj, Canvas, Canvas->sorted_cell_indices_attractiveness
		/**/ [index]);

		int NumFromParent1 = Parent1.dispo->unitsA[ci][cj].num_homes;
		int NumFromParent2 = Parent2.dispo->unitsA[ci][cj].num_homes;
		double ProportionOfFirst = max_double (0, RAND_VAR_HALF_NORMAL_DISTRIBUTION
		/**/ (MaxProportionOfFirst, StdDeviation));

		int NumReceivedHomes = min (cCellHomeLimit, (int)ceil(ProportionOfFirst*NumFromParent1 +
		/**/ (1-ProportionOfFirst)*NumFromParent2));

		// printf("Proportion: %lf ; attempted to merge %d from first and %d from second ; yields %d\n",
		/**/ //ProportionOfFirst, NumFromParent1, NumFromParent2, NumReceivedHomes);

		pChildIndiv->dispo->unitsA[ci][cj].num_homes = NumReceivedHomes;
		NumRemainingHomes -= NumReceivedHomes;
	}
	pChildIndiv->total_score = -DBL_MAX;
	CALCULATE_LOCAL_DENSITIES (Canvas, pChildIndiv->dispo);
	CALCULATE_LOCAL_ENTROPIES (Canvas, pChildIndiv->dispo);
	CALCULATE_SCORES_OF_INDIVIDUAL (pChildIndiv, Canvas);
	return pChildIndiv;
}

void RENEW_POPULATION (canvas Canvas, population Popl, double KeepProportion,
/**/ double MutateProportion, double CrossProportion){

	int NumKeptFromPreviousPop = (int)ceil(KeepProportion*Popl->num_individuals);
	int NumMutatedFromPreviousPop = (int)ceil(MutateProportion*Popl->num_individuals);
	int NumCrossedFromPreviousPop = (int)ceil(CrossProportion*Popl->num_individuals);
	int NumNewlyGenerated = Popl->num_individuals - NumKeptFromPreviousPop
	/**/ - NumMutatedFromPreviousPop - NumCrossedFromPreviousPop;

	int NumHomes = Popl->individualsA[0].dispo->num_inhabitants;

	// keep NumKeptFromPreviousPop and mutate NumMutatedFromPreviousPop of them into
	// new individuals
	for (int index = NumKeptFromPreviousPop; index < NumKeptFromPreviousPop
	/**/ + NumMutatedFromPreviousPop; index++){
		
		individual ReferenceIndiv = Popl->individualsA
		/**/ [Popl->sorted_individual_indices_score[index]];
		individual* pMutatingIndiv = &Popl->individualsA
		/**/ [Popl->sorted_individual_indices_score[index]];

		double RandMutationProportion = 0.15*((double)rand()/(double)RAND_MAX);

		*pMutatingIndiv = *(MUTATE_INDIVIDUAL (Canvas, ReferenceIndiv,
		/**/ RandMutationProportion));
		// FREE_INDIVIDUAL (pMutatingIndiv);
	}

	// cross between the best until
	int parent_1 = 0;
	int parent_2 = 1;
	for (int index = NumKeptFromPreviousPop + NumMutatedFromPreviousPop;
	/**/ index < NumKeptFromPreviousPop + NumMutatedFromPreviousPop + NumCrossedFromPreviousPop;
	/**/ index++){

		individual ParentIndiv1 = Popl->individualsA[Popl->sorted_individual_indices_score
		/**/ [parent_1]];
		individual ParentIndiv2 = Popl->individualsA[Popl->sorted_individual_indices_score
		/**/ [parent_2]];

		individual* pChildIndiv = &Popl->individualsA[Popl->sorted_individual_indices_score
		/**/ [index]];

		double MaxProportionOfFirst = ParentIndiv1.total_score/(ParentIndiv1.total_score +
		/**/ ParentIndiv2.total_score);
		double StdDeviation = 0.5*MaxProportionOfFirst*(double)rand()/(double)RAND_MAX;

		*pChildIndiv = *(CROSS_INDIVIDUALS (Canvas, ParentIndiv1, ParentIndiv2,
		/**/ MaxProportionOfFirst, StdDeviation));

		parent_1++;
		parent_2++;
	}

	// complete the rest with randomly generated individuals
	for (int index = NumKeptFromPreviousPop + NumMutatedFromPreviousPop + NumCrossedFromPreviousPop;
	/**/ index < Popl->num_individuals; index++){

		individual* pCurrentIndiv = &Popl->individualsA
		/**/ [Popl->sorted_individual_indices_score[index]];

		// FREE_INDIVIDUAL (pCurrentIndiv);
		INITIALIZE_INDIVIDUAL (pCurrentIndiv, Canvas, NumHomes);
		// printf ("Individual %d/%d generated randomly.\n", index, Popl->num_individuals);
		// printf ("Attractiveness: %f | Density: %f | Entropy: %f | Cost penalty: %f || Total: %f\n",
		// /**/ index,
		// /**/ pCurrentIndiv->attractiveness_score,
		// /**/ pCurrentIndiv->density_score,
		// /**/ pCurrentIndiv->entropy_score,
		// /**/ pCurrentIndiv->cost_penalty,
		// /**/ pCurrentIndiv->total_score);
		// SDL_RenderClear (RendererGENALGO);
		// SDL_RenderExistingServices (RendererGENALGO, Canvas);
		// SDL_RenderExistingHomes (RendererGENALGO, Canvas, pCurrentIndiv->dispo);
		// SDL_DisplayUnitEntropies (RendererGENALGO, Canvas, pCurrentIndiv->dispo);

		double RandMutationProportion = 0.15*((double)rand()/(double)RAND_MAX);
		*pCurrentIndiv = *(MUTATE_INDIVIDUAL (Canvas, *pCurrentIndiv, RandMutationProportion));
		int parent_1 = rand () % (Popl->num_individuals);
		int parent_2 = parent_1;
		while (parent_2 == parent_1)
			parent_2 = rand () % (Popl->num_individuals);

		individual ParentIndiv1 = Popl->individualsA[Popl->sorted_individual_indices_score
		/**/ [parent_1]];
		individual ParentIndiv2 = Popl->individualsA[Popl->sorted_individual_indices_score
		/**/ [parent_2]];

		*pCurrentIndiv = *(CROSS_INDIVIDUALS (Canvas, ParentIndiv1, ParentIndiv2,
		/**/ 0.5, 0.25));
		// printf ("Individual %d/%d mutated.\n", index, Popl->num_individuals);
		// printf ("Attractiveness: %f | Density: %f | Entropy: %f | Cost penalty: %f || Total: %f\n",
		// /**/ index,
		// /**/ pCurrentIndiv->attractiveness_score,
		// /**/ pCurrentIndiv->density_score,
		// /**/ pCurrentIndiv->entropy_score,
		// /**/ pCurrentIndiv->cost_penalty,
		// /**/ pCurrentIndiv->total_score);
		// printf("\n");
		// SDL_RenderClear (RendererGENALGO);
		// SDL_RenderExistingServices (RendererGENALGO, Canvas);
		// SDL_RenderExistingHomes (RendererGENALGO, Canvas, pCurrentIndiv->dispo);
		// SDL_DisplayUnitEntropies (RendererGENALGO, Canvas, pCurrentIndiv->dispo);
	}

	// resort individuals
	SORT_INDIVIDUAL_INDICES (Popl);
}


void FREE_INDIVIDUAL (individual* pIndiv){

	printf ("freeing individual\n");
	FREE_DISPOSITION (pIndiv->dispo);
}
