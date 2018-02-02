#include <SDL2/SDL.h>
#include <epoxy/gl.h>
#include <epoxy/glx.h>

SDL_Window* window;
SDL_GLContext openglContext;

void initializeWindow (int windowWidth, int windowHeight){
	SDL_Init (SDL_INIT_VIDEO);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);//use core profile
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 4);//OpenGL 4.X
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 0);//OpenGL 4.0
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);//double buffering probably not necessary but I'm not sure I know how to work without it.
	window = SDL_CreateWindow (
		"Monte Carlo",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_OPENGL);
	openglContext = SDL_GL_CreateContext (window);
	SDL_GL_SetSwapInterval (0);//no vsync because why
}

void swapBuffers(){
	SDL_GL_SwapWindow (window);
}

unsigned long long int ticks(){
	return SDL_GetPerformanceCounter();
}

unsigned long long frequency(){
	return SDL_GetPerformanceFrequency();
}

void closeWindow(){
	SDL_GL_DeleteContext (openglContext);
	SDL_DestroyWindow (window);
	SDL_Quit();
}

