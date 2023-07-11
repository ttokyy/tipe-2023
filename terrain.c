#include "terrain.h"


const int cNoiseFrequencyPixels = 200;


// returns smoothstep (lambda)
double SMOOTHSTEP (double lambda){

	return 3*lambda*lambda - 2*lambda*lambda*lambda;
}


// given a noise tiling whose tiles are sized NoiseFrequency and the noise values at each
// tile's corners contained in VertexHeights, returns the height map's value at the point
// of coordinates (PixelX, PixelY) obtained by interpolating between the nearest vertices
// according to the smoothstep function
double F_HEIGHT_OF_PIXEL (double** VertexHeights, int NoiseFrequency, int PixelX, int PixelY){

	// find the position of the tile containing PixelX and PixelY
	int Tile_i = PixelY/NoiseFrequency;
	int Tile_j = PixelX/NoiseFrequency;

	// printf ("%d,%d\n", Tile_i, Tile_j);

	// extract the height at each one of the tile's corners
	double a = VertexHeights[Tile_i][Tile_j];     // top-left
	double b = VertexHeights[Tile_i][Tile_j+1];   // top-right
	double c = VertexHeights[Tile_i+1][Tile_j];   // bottom-left
	double d = VertexHeights[Tile_i+1][Tile_j+1]; // bottom-right

	double i_diff = (double)PixelY/(double)NoiseFrequency - (double)Tile_i;
	double j_diff = (double)PixelX/(double)NoiseFrequency - (double)Tile_j;

	return a + (b-a)*SMOOTHSTEP(j_diff) + (c-a)*SMOOTHSTEP(i_diff) +
	/**/ (a-b-c+d)*SMOOTHSTEP(j_diff)*SMOOTHSTEP(i_diff);
}


// 
void ADD_NOISE_LAYER (canvas Canvas, int NoiseFrequency, double RotationRadians,
/**/ double LayerWeight){

	// calculate number of tiles in length, in width and diagonally to cover window rectangle
	int WidthTiles = cWindowDims[0]/NoiseFrequency;
	int HeightTiles = cWindowDims[1]/NoiseFrequency;
	int DiagonalSizePixels = (int)ceil(sqrt((double)cWindowDims[0]*cWindowDims[0] +
	/**/ (double)cWindowDims[1]*cWindowDims[1]));

	// determine heights of tile vertices randomly
	int InscribingSquareSizeTiles = 2*DiagonalSizePixels/NoiseFrequency;
	double** VertexHeights = (double**) malloc ((InscribingSquareSizeTiles)*sizeof(double*));

	for (int row = 0; row < InscribingSquareSizeTiles; row++){

		VertexHeights[row] = (double*) malloc ((InscribingSquareSizeTiles)*sizeof(double));

		for (int column = 0; column < InscribingSquareSizeTiles; column++){

			VertexHeights[row][column] = LayerWeight*(double)rand()/(double)RAND_MAX;
		}
	}

	// calculate height of each canvas cell's midpoint pixel according to the above values
	int CellX, CellY;

	for (int i = 0; i < Canvas->height_cells; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			ASSIGN_MIDPOINT_OF_CELL (&CellX, &CellY, Canvas, i, j);
			int HalfWidth = cWindowDims[0]/2;
			int HalfHeight = cWindowDims[1]/2;

			// rotate heightmap by RotationRadians
			int RotatedX = cos(RotationRadians)*(CellX-HalfWidth)
			/**/ - sin(RotationRadians)*(CellY-HalfHeight) + DiagonalSizePixels/2;
			int RotatedY = sin(RotationRadians)*(CellX-HalfWidth)
			/**/ + cos(RotationRadians)*(CellY-HalfHeight) + DiagonalSizePixels/2;

			// printf ("%d, %d ---> (%f) %d, %d\n", CellX, CellY, RotationRadians, RotatedX, RotatedY);
			Canvas->cellsA[i][j].terrain_height += F_HEIGHT_OF_PIXEL (VertexHeights,
			/**/ NoiseFrequency, RotatedX, RotatedY);
		}
	}
}


// 
void GENERATE_TERRAIN_HEIGHTMAP (canvas Canvas, double Amplitude, int Frequency, int NumOctaves,
/**/ double Lacunarity, double Persistence, double Exponentiation){

	for (int i = 0; i < Canvas->height_cells; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			Canvas->cellsA[i][j].terrain_height = 0;
		}
	}

	double NoiseFrequency = Frequency;
	double LayerWeight = (1-Persistence);

	for (int layer = 0; layer < NumOctaves; layer++){

		double RandAngle = 2*3.141592*(double)rand()/(double)RAND_MAX;

		ADD_NOISE_LAYER (Canvas, NoiseFrequency, RandAngle, LayerWeight);
		LayerWeight *= Persistence;
		NoiseFrequency = (int)ceil((NoiseFrequency/Lacunarity));
	}
	for (int i = 0; i < Canvas->height_cells; i++){
		for (int j = 0; j < Canvas->width_cells; j++){

			Canvas->cellsA[i][j].terrain_height = Amplitude
			/**/ * pow(Canvas->cellsA[i][j].terrain_height, Exponentiation);
		}
	}
}

void GENERATE_RANDOM_HEIGHTMAP (canvas Canvas){
	for (int i = 0; i < Canvas->height_cells; i++){
		for (int j = 0; j < Canvas->width_cells; j++){
			Canvas->cellsA[i][j].terrain_height = (double)rand()/(double)RAND_MAX;
		}
	}
}