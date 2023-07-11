#include "display.h"

const int cWindowDims[2] = {1920,1080};
const int cGridlineColor[4] = {255, 255, 255, 30};
const int cServiceDisplayColors[7][3] = {
	{239,149,91},
	{157,197,105},
	{52,167,80},
	{122,185,190},
	{136,113,158},
	{133,30,42},
	{100,23,17}
};


const int cTerrainDisplayColors[9][3] = {
	{0,36,172},
	{0,177,249},
	{51,236,56},
	{139,218,45},
	{165,180,55},
	{162,128,49},
	{149,117,71},
	{190,173,136},
	{229,240,245}
};

const double cTerrainCosts[9] = {2000,1900,80,50,60,90,120,160,230};

const double cDeepWaterHeight = 0.25;
const double cShallowWaterHeight = 0.28;
const double cLowGrassHeight = 0.38;
const double cNormalGrassHeight = 0.52;
const double cHighGrassHeight = 0.65;
const double cHillsHeight = 0.75;
const double cLowMountainsHeight = 0.85;
const double cElevatedMountainsHeight = 0.92;
const double cMountainPeaksHeight = 1;

// hyp: 0 <= class < cNumServiceClasses
// assigns to pRed, pGreen and pBlue the color coordinates of service class class's
// display color as determined by cServiceDisplayColors
void ASSIGN_COLOR_OF_SERVICE_CLASS (int class, int* pRed, int* pGreen, int* pBlue){

	*pRed = cServiceDisplayColors[class][0];
	*pGreen = cServiceDisplayColors[class][1];
	*pBlue = cServiceDisplayColors[class][2];
}


// hyp : 0 <= type < 9
// assigns to pRed, pGreen and pBlue the color values of terrain type type's display
// color as determined by cTerrainDisplayColors
void ASSIGN_COLOR_OF_TERRAIN_TYPE (int type, int* pRed, int* pGreen, int* pBlue){

	*pRed = cTerrainDisplayColors[type][0];
	*pGreen = cTerrainDisplayColors[type][1];
	*pBlue = cTerrainDisplayColors[type][2];
}


int TERRAIN_TYPE_OF_HEIGHT (double Height){
	if (Height <= cDeepWaterHeight) return 0;
	else if (Height <= cShallowWaterHeight) return 1;
	else if (Height <= cLowGrassHeight) return 2;
	else if (Height <= cNormalGrassHeight) return 3;
	else if (Height <= cHighGrassHeight) return 4;
	else if (Height <= cHillsHeight) return 5;
	else if (Height <= cLowMountainsHeight) return 6;
	else if (Height <= cElevatedMountainsHeight) return 7;
	else if (Height <= cMountainPeaksHeight) return 8;
}


// displays Canvas's grid with lines of color coordinates cGridlineColor
void SDL_DisplayCanvasGrid (SDL_Renderer* Renderer, canvas Canvas){

	SDL_SetRenderDrawColor (Renderer, cGridlineColor[0], cGridlineColor[1], cGridlineColor[2], cGridlineColor[3]);
	// draw horizontal lines
	for (int i = 0; i < Canvas->height_cells; i++)
		SDL_RenderDrawLine (Renderer, 0, i*Canvas->cell_size, cWindowDims[0], i*Canvas->cell_size);
	// draw vertical lines
	for (int j = 0; j < Canvas->width_cells; j++)
		SDL_RenderDrawLine (Renderer, j*Canvas->cell_size, 0, j*Canvas->cell_size, cWindowDims[1]);
	SDL_RenderPresent (Renderer);
}


// fills each Canvas->cellsA[i][j].fill_square in cyan with an opacity proprtional to the
// terrain height at said cell
void SDL_DisplayTerrainHeights (SDL_Renderer* Renderer, canvas Canvas){

	for (int i = 0; i < Canvas->height_cells ; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			int CyanValue = (int) (255.*Canvas->cellsA[i][j].terrain_height);
			SDL_SetRenderDrawColor (Renderer, 0, CyanValue, CyanValue, 255);
			SDL_RenderFillRect (Renderer, &Canvas->cellsA[i][j].fill_square);
		}
	}
	SDL_RenderPresent (Renderer);
}


// displays the individual values of Canvas->cellsA[i][j].attractiveness in red,
// with brighter shades corresponding to higher attractiveness levels
void SDL_DisplayCellAttractivenesses (SDL_Renderer* Renderer, canvas Canvas){

	for (int i = 0; i < Canvas->height_cells ; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			if (Canvas->cellsA[i][j].service_presence_class == -1){
				int RedBlueValue = (int) (Canvas->cellsA[i][j].attractiveness*(255./1000.));
				if (RedBlueValue < 0){
					SDL_SetRenderDrawColor (Renderer, 0, 0, -RedBlueValue, 255);
				}
				else
					SDL_SetRenderDrawColor (Renderer, RedBlueValue, 0, 0, 255);
				SDL_RenderFillRect (Renderer, &Canvas->cellsA[i][j].fill_square);
			}
		}
	}
	SDL_RenderPresent (Renderer);
}


// fills each Canvas->cellsA[i][j].fill_square in green with an opacity proportional
// to the local densities, i.e. Dispo->unitsA[i][j].local_density
void SDL_DisplayUnitDensities (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo){

	for (int i = 0; i < Canvas->height_cells ; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			int GreenValue = (int) (255.0*(Dispo->unitsA[i][j].local_density)/Dispo->max_possible_density);
			SDL_SetRenderDrawColor (Renderer, 0, GreenValue, 0, 255);
			SDL_RenderFillRect (Renderer, &Canvas->cellsA[i][j].fill_square);
		}
	}
	SDL_RenderPresent (Renderer);
}


// fills each Canvas->cellsA[i][j].fill_square in magenta with an opacity proportional
// to the local entropy value stored in Dispo->unitsA[i][j]
void SDL_DisplayUnitEntropies (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo){

	double MaxEntropy = log(cCellHomeLimit);

	for (int i = 0; i < Canvas->height_cells ; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			int MagentaValue = (int) (255.0*(Dispo->unitsA[i][j].local_entropy)/MaxEntropy);
			SDL_SetRenderDrawColor (Renderer, MagentaValue, 0, MagentaValue, 255);
			SDL_RenderFillRect (Renderer, &Canvas->cellsA[i][j].fill_square);
		}
	}
	SDL_RenderPresent (Renderer);
}


// renders the services which have been drawn and filled on Canvas
void SDL_RenderExistingServices (SDL_Renderer* Renderer, canvas Canvas){

	l_list BorderPixels = Canvas->service_border_pixelsL;
	l_list InteriorCells = Canvas->service_interior_cellsL;
	int i, j;
	int Red, Green, Blue;

	// render borders
	while (BorderPixels != NULL){

		ASSIGN_COLOR_OF_SERVICE_CLASS (BorderPixels->spec, &Red, &Green, &Blue);
		SDL_SetRenderDrawColor (Renderer, Red, Green, Blue, 255);
		ASSIGN_CELL_OF_PIXEL (&i, &j, Canvas, BorderPixels->x, BorderPixels->y);
		SDL_RenderFillRect (Renderer, &Canvas->cellsA[i][j].fill_square);

		BorderPixels = BorderPixels->next;
	}
	// render interiors
	while (InteriorCells != NULL){

		ASSIGN_COLOR_OF_SERVICE_CLASS (InteriorCells->spec, &Red, &Green, &Blue);
		SDL_SetRenderDrawColor (Renderer, Red, Green, Blue, 255);
		SDL_RenderFillRect (Renderer, &Canvas->cellsA[InteriorCells->x][InteriorCells->y].fill_square);
		
		InteriorCells = InteriorCells->next;
	}
	SDL_RenderPresent (Renderer);
}


// colors each cell Canvas->cellsA[i][j] in white with an opacity proportional to
// the number of homes contained in Dispo->unitsA[i][j]
void SDL_RenderExistingHomes (SDL_Renderer* Renderer, canvas Canvas, disposition Dispo){

	for (int i = 0; i < Canvas->height_cells ; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			int WhiteValue = (int) (255.0*((double) Dispo->unitsA[i][j].num_homes)
			/**/ /(double)cCellHomeLimit);
			SDL_SetRenderDrawColor (Renderer, WhiteValue, WhiteValue, WhiteValue, 255);
			SDL_RenderFillRect (Renderer, &Canvas->cellsA[i][j].fill_square);
		}
	}
	SDL_RenderPresent (Renderer);
}


void SDL_RenderTerrain (SDL_Renderer* Renderer, canvas Canvas){

	int Red, Green, Blue;

	for (int i = 0; i < Canvas->height_cells ; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			int TerrainType = TERRAIN_TYPE_OF_HEIGHT (Canvas->cellsA[i][j].terrain_height);
			ASSIGN_COLOR_OF_TERRAIN_TYPE (TerrainType, &Red, &Green, &Blue);

			SDL_SetRenderDrawColor (Renderer, Red, Green, Blue, 255);
			SDL_RenderFillRect (Renderer, &Canvas->cellsA[i][j].fill_square);
		}
	}
	SDL_RenderPresent (Renderer);
}


// hyp: the current SDL render draw color is the display color of service class class
// && cell (i,j) of Canvas is surrounded by a closed boundary of the same class
// if Canvas->cellsA[i][j] is a valid cell, fills it, marks it as occupied by service class
// class and adds it to updates Canvas->service_interior_cellsL
// then, applies itself recursively on all neighboring cells which have not yet been
// marked as occupied by class, i.e. which have yet to be filled
void SDL_FloodFillService (SDL_Renderer* Renderer, canvas Canvas, int class, int i, int j){

	if (IS_CELL_IN_CANVAS (Canvas, i, j)){

		// fill starting cell and update canvas cell attributes and service interiors list
		SDL_RenderFillRect (Renderer, &Canvas->cellsA[i][j].fill_square);
		Canvas->cellsA[i][j].service_presence_class = class;
		l_insert (&Canvas->service_interior_cellsL, i, j, class);

		// recursive calls to all four neighboring cells
		if (Canvas->cellsA[i+1][j].service_presence_class != class){
			Canvas->cellsA[i+1][j].service_presence_class = class;
			SDL_FloodFillService (Renderer, Canvas, class, i+1, j);
		}
		if (Canvas->cellsA[i][j+1].service_presence_class != class){
			Canvas->cellsA[i+1][j].service_presence_class = class;
			SDL_FloodFillService (Renderer, Canvas, class, i, j+1);
		}
		if (Canvas->cellsA[i][j-1].service_presence_class != class){
			Canvas->cellsA[i+1][j].service_presence_class = class;
			SDL_FloodFillService (Renderer, Canvas, class, i, j-1);
		}
		if (Canvas->cellsA[i-1][j].service_presence_class != class){
			Canvas->cellsA[i+1][j].service_presence_class = class;
			SDL_FloodFillService (Renderer, Canvas, class, i-1, j);
		}
	}
}


// literally does nothing (made for convenience and ease of operation of SDL keypress
// registers)
void SDL_DoNothing (void){}