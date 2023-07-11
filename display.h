#ifndef DISPLAY_H
#define DISPLAY_H

#include "canvas.h"
#include "dispo.h"
#include "terrain.h"



// ----- GLOBAL CONSTANTS ----- //

extern const int cGridlineColor[4];

extern const int cServiceDisplayColors[7][3]; // should have size cNumServiceClass x 3
extern const int cTerrainDisplayColors[9][3];

extern const double cTerrainCosts[9];

extern const double cDeepWaterHeight;
extern const double cShallowWaterHeight;
extern const double cLowGrassHeight;
extern const double cNormalGrassHeight;
extern const double cHighGrassHeight;
extern const double cHillsHeight;
extern const double cLowMountainsHeight;
extern const double cElevatedMountainsHeight;
extern const double cMountainPeaksHeight;


// ----- FUNCTIONS ----- //

// utilitary
void ASSIGN_COLOR_OF_SERVICE_CLASS (int class, int* pRed, int* pGreen, int* pBlue);
void ASSIGN_COLOR_OF_TERRAIN_TYPE (int type, int* pRed, int* pGreen, int* pBlue);
int TERRAIN_TYPE_OF_HEIGHT (double Height);

// displaying attributes
void SDL_DisplayCanvasGrid (SDL_Renderer* Renderer, canvas Canvas);
void SDL_DisplayTerrainHeights (SDL_Renderer* Renderer, canvas Canvas);
void SDL_DisplayCellAttractivenesses (SDL_Renderer* Renderer, canvas Canvas);
void SDL_DisplayUnitDensities (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo);
void SDL_DisplayUnitEntropies (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo);

// rendering objects
void SDL_RenderExistingServices (SDL_Renderer* Renderer, canvas Canvas);
void SDL_RenderExistingHomes (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo);
void SDL_RenderTerrain (SDL_Renderer* Renderer, canvas Canvas);

// filling
void SDL_FloodFillService (SDL_Renderer* Renderer, canvas Canvas, int class, int i, int j);



// unused
void SDL_DoNothing (void);


#endif