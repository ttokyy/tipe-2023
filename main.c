#include "canvas.h"
#include "dispo.h"
#include "gen-algo.h"

#include "display.h"
#include "interactions.h"

int main (int argc, char argv[]){

	// initialize srand
	srand(time(NULL));
	
	// initialize and configure SDL
	SDL_Window* Window = NULL;
	SDL_Renderer* Renderer = NULL;

	if (SDL_Init (SDL_INIT_VIDEO) != 0){
		SDL_Log ("ERROR : SDL initialization failed > %s\n", SDL_GetError ());
		exit (EXIT_FAILURE);
	}

	Window = SDL_CreateWindow ("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_MAXIMIZED);
	if (Window == NULL){
		SDL_Log ("ERROR : SDL window creation failed > %s\n", SDL_GetError ());
		exit (EXIT_FAILURE);
	}

	Renderer = SDL_CreateRenderer(Window,-1,SDL_RENDERER_SOFTWARE);
    if (Renderer == NULL) {
    	SDL_Log ("ERROR : SDL renderer creation failed > %s\n", SDL_GetError ());
		exit (EXIT_FAILURE);
    }
    SDL_bool PRG_RUN = SDL_TRUE;
    
    SDL_SetRenderDrawBlendMode(Renderer,SDL_BLENDMODE_ADD);
    SDL_RenderPresent (Renderer);



    RendererGENALGO = Renderer;
    double KeepProportion;
    double MutateProportion;


    // initialize canvas and attributes
    canvas MainCanvas = CREATE_CANVAS (cCellSizePixels, cWindowDims);
    int CurrentServiceClass = 0;
    int currentEntertainType = 0;

    // declare/initialize test disposition and population
    disposition TestDispo = CREATE_CANVAS_DISPOSITION (MainCanvas);
    individual TestIndiv;
    individual TestIndiv_bis;
    population TestPopl;

    // declare mouse interaction variables
    bool IsMouseButtonClicked = false;
    int MouseX, MouseY;

    // declare drawing and filling varibales
    int DrawRed;
    int DrawGreen;
    int DrawBlue;

    int Fill_row;
    int Fill_column;



    SDL_Event Event;

    // program execution
    while(PRG_RUN){

        while (SDL_PollEvent(&Event)){

            switch (Event.type){

            	case (SDL_KEYDOWN) :

            		switch (Event.key.keysym.sym){

            			case SDLK_q :
            				USER_CLEAR_SCREEN (Renderer);
            				continue;
            			case SDLK_w :
            				USER_CYCLE_SERVICE_CLASS_PREV (Renderer, &CurrentServiceClass,
            					&DrawRed, &DrawBlue, &DrawGreen);
            				continue;
            			case SDLK_e :
            				USER_CYCLE_SERVICE_CLASS_NEXT (Renderer, &CurrentServiceClass,
            					&DrawRed, &DrawBlue, &DrawGreen);
            				continue;
            			case SDLK_r :
            				USER_RERENDER_SERVICES (Renderer, MainCanvas);
            				continue;
            			case SDLK_t :
            				USER_CALCULATE_AND_DISPLAY_ATTRACTIVENESSES (Renderer,
            					MainCanvas);
            				continue;
            			case SDLK_y :
            				USER_DISPLAY_ATTRACTIVENESSES (Renderer, MainCanvas);
            				continue;
            			case SDLK_f :
            				USER_FILL_SERVICE (Renderer, &MouseX, &MouseY, MainCanvas,
            					&Fill_row, &Fill_column, CurrentServiceClass);
            				continue;
            			case SDLK_u :
            				USER_INITIALIZE_DISPOSITION_AND_DISPLAY_HOMES (Renderer,
            					MainCanvas, TestDispo);
            				continue;
            			case SDLK_i :
            				USER_CALCULATE_AND_DISPLAY_DENSITIES (Renderer,
            					MainCanvas, TestDispo);
            				continue;
            			case SDLK_o :
            				USER_INITIALIZE_AND_SORT_POPULATION (MainCanvas, &TestPopl);
            				continue;
            			case SDLK_p :
            				USER_DISPLAY_POPULATION_INDIVIDUAL_DESC_SCORE (Renderer, MainCanvas,
            					TestPopl);
            				continue;
            			case SDLK_a :
            				USER_INITIALIZE_AND_DISPLAY_INIDIVIDUAL (MainCanvas,
            					TestDispo, &TestIndiv);
            				continue;
            			case SDLK_s :
            				USER_MUTATE_AND_DISPLAY_INDIVIDUAL (Renderer, MainCanvas, &TestIndiv);
            				continue;
            			/* case SDLK_d :
            				USER_RENEW_POPULATION (MainCanvas, TestPopl);
            				continue; */	
            			/* case SDLK_g :
            				USER_LAUNCH_GENETIC_ALGORITHM (Renderer, MainCanvas);
            				continue; */
            			case SDLK_h :
            				USER_CALCULATE_AND_DISPLAY_ENTROPIES (Renderer, MainCanvas,
            					TestDispo);
            				continue;
            			case SDLK_j :
            				USER_COMPUTE_AND_DISPLAY_HEIGHTS (Renderer, MainCanvas);
            				continue;
            			case SDLK_k :
            				USER_DISPLAY_TERRAIN (Renderer, MainCanvas);
            				continue;
            			case SDLK_l :
            				USER_GENETIC_ALGORITHM (Renderer, MainCanvas);
            				continue;
            			case SDLK_z :
            				USER_INITIALIZE_POPULATION (MainCanvas, &TestPopl, &KeepProportion,
            					&MutateProportion);
            				continue;
            			case SDLK_x :
            				USER_GENETIC_ALGORITHM_STEP (MainCanvas, TestPopl, KeepProportion,
            					MutateProportion);
            				continue;
            			case SDLK_c :
            				USER_INITIALIZE_AND_DISPLAY_INDIVIDUAL_AND_DISPO (Renderer, MainCanvas,
            					&TestIndiv);
            				continue;
            			case SDLK_v :
            				USER_INITIALIZE_AND_DISPLAY_INDIVIDUAL_AND_DISPO (Renderer, MainCanvas,
            					&TestIndiv_bis);
            				continue;
            			case SDLK_b :
            				USER_CROSS_AND_DISPLAY_INDIVIDUALS (Renderer, MainCanvas,
            					&TestIndiv, &TestIndiv_bis);
            				continue;
            			case SDLK_n :
            				USER_COMPUTE_AND_DISPLAY_HEIGHTS (Renderer, MainCanvas);
            				continue;

            			default:
            				continue;
            		}
            
            	case (SDL_MOUSEBUTTONDOWN) :
            		IsMouseButtonClicked = true;
            		continue;

            	case (SDL_MOUSEBUTTONUP) :
            		IsMouseButtonClicked = false;
            		continue;

				case (SDL_MOUSEMOTION) :
					if (IsMouseButtonClicked) USER_DRAW_SERVICE (Renderer, &DrawRed,
						&DrawBlue, &DrawGreen, &MouseX, &MouseY, MainCanvas, &Fill_row,
						&Fill_column, CurrentServiceClass);
					continue;
            			
		        case SDL_QUIT:
		                PRG_RUN = SDL_FALSE;
		                SDL_DestroyRenderer (Renderer);
		                SDL_DestroyWindow (Window);
		                break;
		            default:
		                continue;
		    }
		}
	}

	return EXIT_SUCCESS;
}