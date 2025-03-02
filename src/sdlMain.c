// Code provided by :
// https://github.com/libsdl-org/SDL/blob/main/examples/renderer/01-clear/clear.c

#include <GPbase.h>


// renderer is used to draw into the window every frame.
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

// This function runs once on startup
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", DEFAULT_WIDTH, DEFAULT_HEIGHT, 0, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

	printf("%d extensions supported\n", extensionCount);

	return SDL_APP_CONTINUE; // The init function ran successfully
}

// This function runs when a new event (mouse input, keypress, etc) occurs.
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS; // Ends the program, reports a success to the OS. "Program ran successfully"
	}

	return SDL_APP_CONTINUE; // Carry on with the program
}

// This function runs once per frame. It is the beating heart behind the program
SDL_AppResult SDL_AppIterate(void* appstate) {
	const double now = ((double)SDL_GetTicks()) / 1000.0f; // converts from ms to s

	// The RGB values of the color that is drawn. The sin functions make it fade
	const float red = (float)(0.5 + 0.5 * SDL_sin(now));
	const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
	const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
	SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT); // new color, full alpha

	// Clears the window with the draw color
	SDL_RenderClear(renderer);

	// Put the newly-cleared rendering to the screen.
	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE; // Carry on with the program
}

// This function runs once when the program is closed.
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	// SDL cleans up the window/renderer for us
}