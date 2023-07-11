#ifndef TERRAIN_H
#define TERRAIN_H

#include "canvas.h"



// ----- FUNCTIONS ----- //

double SMOOTHSTEP (double lambda);
double F_HEIGHT_OF_PIXEL (double** VertexHeights, int NoiseFrequency, int PixelX, int PixelY);
void ADD_NOISE_LAYER (canvas Canvas, int NoiseFrequency, double RotationRadians,
/**/ double LayerWeight);
void GENERATE_TERRAIN_HEIGHTMAP (canvas Canvas, double Amplitude, int Frequency,
/**/ int NumOctaves, double Lacunarity, double Persistence, double Exponentiation);



// unused
double F (canvas Canvas, double** VertexHeights, int PixelX, int PixelY);
double F_2 (canvas Canvas, double** VertexHeights, int NoiseTileSize, int PixelX, int PixelY);
void COMPUTE_HEIGHTS_2 (canvas Canvas, int NoiseTileSize, double Rotation, double Weight);
void COMPUTE_BASE (canvas Canvas);
void OK_EPIC (canvas Canvas, int NoiseTileSize, int NumLayers);
void COMPUTE_HEIGHTS (canvas Canvas);

void GENERATE_RANDOM_HEIGHTMAP (canvas Canvas);
#endif