#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.h"
#include "renderer.h"
#include "imgui/imgui.h"

class Application {

public:

	Window window;
	Renderer render;

	const unsigned int SCR_WIDTH = 1600;
	const unsigned int SCR_HEIGHT = 900;

	Application() {

		window = Window(SCR_WIDTH, SCR_HEIGHT, "window");
		render = Renderer(SCR_WIDTH, SCR_HEIGHT);
	}

	void init() {

		window.init_window();
		render.init();
	}


};


#endif