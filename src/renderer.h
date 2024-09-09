#ifndef RENDERER_H
#define RENDERER_H

#include "quad.h"

class Renderer {

public:

	Quad screen_quad;
	unsigned int quadVAO;
	unsigned int quadVBO;
	unsigned int framebuffer;
	unsigned int rbo;
	unsigned int textureColorbuffer;

	unsigned int width;
	unsigned int height;

	float *pixels;
	int sizeOfBuffer;

	Renderer() {}

	Renderer(unsigned int w, unsigned int h) {
		width = w;
		height = h;
		screen_quad = Quad();
		sizeOfBuffer = w * h * 3;
		pixels = new float[w*h * 3];
	}


	void init() {

		// screen quad VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad.quadVertices), &screen_quad.quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


		// framebuffer configuration
		// -------------------------
		
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// create a color attachment texture
		
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
																									  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDepthFunc(GL_LESS);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


	}

	template <size_t N>
	void clearScreen(const int (&col)[N]) {
		static_assert(N == 3, "Function requires an array of size 3");
		for (int i = 0; i < sizeOfBuffer; i+=3) {
			this->pixels[i] = col[0];
			this->pixels[i + 1] = col[1];
			this->pixels[i + 2] = col[2];
		}
	}

	//simple parametric line drawing
	void renderLine(const int x0, const int y0, 
					const int x1, const int y1) {

		for (float t = 0.; t<1.; t += 0.01) {
			int x = x0 + (x1 - x0)*t;
			int y = y0 + (y1 - y0)*t;

			//(x,y) points can be defined as negative in 2D cartesian space. The array that holds the data is 1D in only positive indices
			//this maps the negative quadrant from cartesian space to the positive position in the array. i.e. (-50, 50) in a 100x100 cartesian space
			//will get mapped to (0,0) in the array. Both are equivelant in representing lower left corner in both spaces.
			int normX = x + width / 2;
			int normY = y + height / 2;

			//if values are larger than (SCR_WIDTH, SCR_HEIGHT) or less than (0,0) then we will be writing outside the array bounds.
			//this clamps the values
			if (normX < 0) normX = 0;
			if (normX > width - 1) normX = width - 1;
			if (normY < 0) normY = 0;
			if (normY > height - 1) normY = height - 1;


			this->pixels[(width * normY + normX) * 3] = 0.0f;
			this->pixels[(width * normY + normX) * 3 + 1] = 0.0f;
			this->pixels[(width * normY + normX) * 3 + 2] = 1.0f;

		}
	}

	void renderTriangle(const int p1x, const int p1y, 
						const int p2x, const int p2y, 
						const int p3x, const int p3y) {
		renderLine(p1x, p1y, p2x, p2y);
		renderLine(p2x, p2y, p3x, p3y);
		renderLine(p3x, p3y, p1x, p1y);
	}


};

#endif