#ifndef INTERACTIONS_H
#define INTERACTIONS_H

#include "canvas.h"
#include "terrain.h"
#include "display.h"
#include "dispo.h"
#include "gen-algo.h"

void USER_CLEAR_SCREEN (SDL_Renderer* Renderer);

void USER_CYCLE_SERVICE_CLASS_PREV (SDL_Renderer* Renderer, int* pServiceClass, int* pRed, int* pBlue, int* pGreen);
void USER_CYCLE_SERVICE_CLASS_NEXT (SDL_Renderer* Renderer, int* pServiceClass, int* pRed, int* pBlue, int* pGreen);

void USER_DRAW_SERVICE (SDL_Renderer* Renderer, int* pRed, int* pGreen, int* pBlue,
/**/ int* pMouseX, int* pMouseY, canvas Canvas, int* pFill_i, int* pFill_j, int ServiceClass);
void USER_FILL_SERVICE (SDL_Renderer* Renderer, int* pMouseX, int* pMouseY,
/**/ canvas Canvas, int* pFill_i, int* pFill_j, int ServiceClass);
void USER_RERENDER_SERVICES (SDL_Renderer* Renderer, canvas Canvas);


void USER_CALCULATE_AND_DISPLAY_ATTRACTIVENESSES (SDL_Renderer* Renderer, canvas Canvas);
void USER_DISPLAY_ATTRACTIVENESSES (SDL_Renderer* Renderer, canvas Canvas);

void USER_INITIALIZE_DISPOSITION_AND_DISPLAY_HOMES (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo);
void USER_CALCULATE_AND_DISPLAY_DENSITIES (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo);

void USER_INITIALIZE_AND_SORT_POPULATION (canvas Canvas, population* pPopl);
void USER_RENEW_POPULATION (canvas Canvas, population Popl);

void USER_DISPLAY_POPULATION_INDIVIDUAL_DESC_SCORE (SDL_Renderer* Renderer, canvas Canvas,
/**/ population Popl);

void USER_INITIALIZE_AND_DISPLAY_INIDIVIDUAL (canvas Canvas, disposition Dispo,
/**/ individual* pIndiv);
void USER_MUTATE_AND_DISPLAY_INDIVIDUAL (SDL_Renderer* Renderer, canvas Canvas,
/**/ individual* pIndiv);

// void USER_LAUNCH_GENETIC_ALGORITHM (SDL_Renderer* Renderer, canvas Canvas);
void USER_CALCULATE_AND_DISPLAY_ENTROPIES (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo);




void USER_COMPUTE_AND_DISPLAY_HEIGHTS (SDL_Renderer* Renderer, canvas Canvas);
void USER_DISPLAY_TERRAIN (SDL_Renderer* Renderer, canvas Canvas);


void INITIALIZE_AND_EVOLVE_POPULATION (SDL_Renderer* Renderer, canvas Canvas);
void USER_GENETIC_ALGORITHM (SDL_Renderer* Renderer, canvas	Canvas);



void USER_INITIALIZE_POPULATION (canvas Canvas, population* pPopl, double* pKeepProportion,
/**/ double* pMutateProportion);
void USER_GENETIC_ALGORITHM_STEP (canvas Canvas, population Popl,
/**/ double KeepProportion, double MutateProportion);

void USER_CROSS_AND_DISPLAY_INDIVIDUALS (SDL_Renderer* Renderer, canvas Canvas,
/**/ individual* pIndiv1, individual* pIndiv2);
void USER_INITIALIZE_AND_DISPLAY_INDIVIDUAL_AND_DISPO (SDL_Renderer* Renderer,
	canvas Canvas, individual* pIndiv);

#endif