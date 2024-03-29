// RetroEngine.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS//need for tinygltf
#define RASTER

#include "src/shader.h"
#include <glad/glad.h>
#include "src/application.h"

//un comment when not using tinygltf
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//uncomment when using tinygltf
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "src/tiny_gltf.h"

#include <iostream>

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) //used for tinygltf

// settings
constexpr unsigned int SCR_WIDTH = 1600;
constexpr unsigned int SCR_HEIGHT = 900;

float pixels[SCR_HEIGHT * SCR_HEIGHT * 3];

tinygltf::Model modelobj;


struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];
};

struct mesh
{
	std::vector<triangle> tris;
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

mesh meshCube;
mat4x4 matProj;

float fTheta;

void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}




bool loadModel(tinygltf::Model &model, const char *filename) {
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	//bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
	
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}



void bindMesh(std::map<int, GLuint>& vbos, std::map<int, std::map<int, GLuint>>& vaos, std::map<int, std::map<int, GLuint>>& texs, tinygltf::Model &model, tinygltf::Mesh &mesh, size_t child) {


	
	/*
	for (size_t i = 0; i < model.bufferViews.size(); ++i) {
		const tinygltf::BufferView &bufferView = model.bufferViews[i];
		if (bufferView.target == 0) {  // TODO impl drawarrays
			std::cout << "WARN: bufferView.target is zero" << std::endl;
			continue;  
		}
		const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
		//std::cout << "bufferview.target " << bufferView.target << std::endl;

		GLuint vbo;
		glGenBuffers(1, &vbo);
		vbos[i] = vbo;
		glBindBuffer(bufferView.target, vbo);

		//std::cout << "buffer.data.size = " << buffer.data.size()
			//<< ", bufferview.byteOffset = " << bufferView.byteOffset
			//<< std::endl;

		glBufferData(bufferView.target, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
	}
	*/
	std::map<int, GLuint> temp_vao;
	std::map<int, GLuint> temp_tex;

	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		//vao for the given mesh
		GLuint vao;
		glGenVertexArrays(1, &vao);
		temp_vao[i] = vao;
		glBindVertexArray(vao);

		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		//bind the index buffer and its target type and the data in it
		GLuint index_vbo;
		glGenBuffers(1, &index_vbo);
		glBindBuffer(model.bufferViews[indexAccessor.bufferView].target, index_vbo);
		glBufferData(model.bufferViews[indexAccessor.bufferView].target, model.bufferViews[indexAccessor.bufferView].byteLength,
			&model.buffers[model.bufferViews[indexAccessor.bufferView].buffer].data.at(0) + model.bufferViews[indexAccessor.bufferView].byteOffset, GL_STATIC_DRAW);

		//glBindBuffer(model.bufferViews[indexAccessor.bufferView].target, vbos[indexAccessor.bufferView]);

		for (auto &attrib : primitive.attributes) {
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);

			//model.bufferViews[accessor.bufferView].target;
			//glBindBuffer(model.bufferViews[accessor.bufferView].target, vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR) {
				size = accessor.type;
			}

			//std::cout << accessor.type << " " << size << std::endl;

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0) vaa = 0;
			if (attrib.first.compare("NORMAL") == 0) vaa = 1;
			if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
			if (attrib.first.compare("TANGENT") == 0) vaa = 3;
			//Positions
			if (vaa == 0) {
				GLuint vertex_vbo;
				glGenBuffers(1, &vertex_vbo);
				glBindBuffer(model.bufferViews[accessor.bufferView].target, vertex_vbo);
				glBufferData(model.bufferViews[accessor.bufferView].target, model.bufferViews[accessor.bufferView].byteLength,
					&model.buffers[model.bufferViews[accessor.bufferView].buffer].data.at(0) + model.bufferViews[accessor.bufferView].byteOffset, GL_STATIC_DRAW);

				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));
			}
			else if (vaa == 1) {//Normals
				GLuint normal_vbo;
				glGenBuffers(1, &normal_vbo);
				glBindBuffer(model.bufferViews[accessor.bufferView].target, normal_vbo);
				glBufferData(model.bufferViews[accessor.bufferView].target, model.bufferViews[accessor.bufferView].byteLength,
					&model.buffers[model.bufferViews[accessor.bufferView].buffer].data.at(0) + model.bufferViews[accessor.bufferView].byteOffset, GL_STATIC_DRAW);

				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));
			}
			else if (vaa == 2) {//texture coords
				GLuint texture_vbo;
				glGenBuffers(1, &texture_vbo);
				glBindBuffer(model.bufferViews[accessor.bufferView].target, texture_vbo);
				glBufferData(model.bufferViews[accessor.bufferView].target, model.bufferViews[accessor.bufferView].byteLength,
					&model.buffers[model.bufferViews[accessor.bufferView].buffer].data.at(0) + model.bufferViews[accessor.bufferView].byteOffset, GL_STATIC_DRAW);

				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));

			}else if(vaa == 3){//tangents
				GLuint tangent_vbo;
				glGenBuffers(1, &tangent_vbo);
				glBindBuffer(model.bufferViews[accessor.bufferView].target, tangent_vbo);
				glBufferData(model.bufferViews[accessor.bufferView].target, model.bufferViews[accessor.bufferView].byteLength,
					&model.buffers[model.bufferViews[accessor.bufferView].buffer].data.at(0) + model.bufferViews[accessor.bufferView].byteOffset, GL_STATIC_DRAW);

				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));
			}
			else {
				std::cout << "not supported attribute" << std::endl;
			}
			/*
			if (vaa > -1) {
				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));
			}
			else
				std::cout << "vaa missing: " << attrib.first << std::endl;
			*/
		}

		
		if (model.textures.size() > 0) {
			// fixme: Use material's baseColor
			tinygltf::Texture &tex = model.textures[0];
			//tinygltf::Texture &tex = model.textures[1];

			if (tex.source > -1) {

				GLuint texid;
				
				glGenTextures(1, &texid);
				temp_tex[i] = texid;
				tinygltf::Image &image = model.images[0];
				if (!(model.materials[mesh.primitives[i].material].pbrMetallicRoughness.baseColorTexture.index == -1)) {
					image = model.images[model.textures[model.materials[mesh.primitives[i].material].pbrMetallicRoughness.baseColorTexture.index].source];
				}

				glBindTexture(GL_TEXTURE_2D, temp_tex[i]);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				GLenum format = GL_RGBA;

				if (image.component == 1) {
					format = GL_RED;
				}
				else if (image.component == 2) {
					format = GL_RG;
				}
				else if (image.component == 3) {
					format = GL_RGB;
				}
				else {
					// ???
				}

				GLenum type = GL_UNSIGNED_BYTE;
				if (image.bits == 8) {
					// ok
				}
				else if (image.bits == 16) {
					type = GL_UNSIGNED_SHORT;
				}
				else {
					// ???
				}

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
					format, type, &image.image.at(0));
			}
		}
		
		
		vaos[child] = temp_vao;
		texs[child] = temp_tex;
		
	}
}

// bind models
void bindModelNodes(std::map<int, GLuint>& vbos, std::map<int, std::map<int, GLuint>>& vaos, std::map<int, std::map<int, GLuint>>& texs, tinygltf::Model &model,
	tinygltf::Node &node, size_t current_node) {


	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		bindMesh(vbos, vaos, texs, model, model.meshes[node.mesh], current_node);
	}

	for (size_t i = 0; i < node.children.size(); i++) {
		assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
		// TODO: put a list to hold the vaos and tex for the meshes attached
		bindModelNodes(vbos, vaos, texs, model, model.nodes[node.children[i]], node.children[i]);
	}
}


std::pair<std::map<int, std::map<int, GLuint>>, std::map<int, std::map<int, GLuint>>> bindModel(tinygltf::Model &model) {
	std::map<int, GLuint> vbos;
	std::map<int, std::map<int, GLuint>> vaos;
	std::map<int, std::map<int, GLuint>> texs;

	const tinygltf::Scene &scene = model.scenes[model.defaultScene];

	//std::cout << scene.nodes.size() << std::endl;

	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
		//std::cout << i << std::endl;
		bindModelNodes(vbos, vaos, texs, model, model.nodes[scene.nodes[i]], 0);
	}

	glBindVertexArray(0);

	// cleanup vbos but do not delete index buffers yet
	/*
	for (auto it = vbos.cbegin(); it != vbos.cend();) {
		tinygltf::BufferView bufferView = model.bufferViews[it->first];
		if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
			glDeleteBuffers(1, &vbos[it->first]);
			vbos.erase(it++);
		}
		else {
			++it;
		}
	}
	*/
	
	

	return { vaos, texs };
}

void drawMesh(const std::map<int, GLuint>& texs, const std::map<int, GLuint>& vaos,
	tinygltf::Model &model, tinygltf::Mesh &mesh) {

	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
		glBindVertexArray(vaos.at(i));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texs.at(i));
		//glBindBuffer(model.bufferViews[indexAccessor.bufferView].target, vbos.at(indexAccessor.bufferView));

		if (model.bufferViews[indexAccessor.bufferView].target == 34963) {
			glDrawElements(primitive.mode, indexAccessor.count,
				indexAccessor.componentType,
				BUFFER_OFFSET(indexAccessor.byteOffset));
		}
		else {
			glDrawArrays(primitive.mode, indexAccessor.byteOffset,
				indexAccessor.count);
		}

		
	}
}

// recursively draw node and children nodes of model
void drawModelNodes(const std::pair<std::map<int, std::map<int, GLuint>>, std::map<int, std::map<int, GLuint>>>& vaoAndEbos, tinygltf::Model &model, tinygltf::Node &node, size_t current_node) {

	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		drawMesh(vaoAndEbos.second.at(current_node), vaoAndEbos.first.at(current_node), model, model.meshes[node.mesh]);
	}

	for (size_t i = 0; i < node.children.size(); i++) {
		drawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]], node.children[i]);
	}

}
void drawModel(const std::pair<std::map<int, std::map<int, GLuint>>, std::map<int, std::map<int, GLuint>>>& vaoAndEbos, tinygltf::Model &model) {

	const tinygltf::Scene &scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		drawModelNodes(vaoAndEbos, model, model.nodes[scene.nodes[i]], 0);
	}

	glBindVertexArray(0);
}

void dbgModel(tinygltf::Model &model) {
	for (auto &mesh : model.meshes) {
		std::cout << "mesh : " << mesh.name << std::endl;
		for (auto &primitive : mesh.primitives) {
			const tinygltf::Accessor &indexAccessor =
				model.accessors[primitive.indices];

			std::cout << "indexaccessor: count " << indexAccessor.count << ", type "
				<< indexAccessor.componentType << std::endl;

			tinygltf::Material &mat = model.materials[primitive.material];
			for (auto &mats : mat.values) {
				std::cout << "mat : " << mats.first.c_str() << std::endl;
			}

			for (auto &image : model.images) {
				std::cout << "image name : " << image.uri << std::endl;
				std::cout << "  size : " << image.image.size() << std::endl;
				std::cout << "  w/h : " << image.width << "/" << image.height
					<< std::endl;
			}

			std::cout << "indices : " << primitive.indices << std::endl;
			std::cout << "mode     : "
				<< "(" << primitive.mode << ")" << std::endl;

			for (auto &attrib : primitive.attributes) {
				std::cout << "attribute : " << attrib.first.c_str() << std::endl;
			}
		}
	}
}

#ifndef RASTER
int main()
{
	//create and init the whole application
	Application app;
	app.init();

	//std::string filename = "resources\\models\\castle3\\scene.gltf";
	//std::string filename = "resources\\models\\Sponza\\glTF\\Sponza.gltf";
	//std::string filename = "resources\\models\\scan\\scene.gltf";
	//std::string filename = "resources\\models\\construction\\scene.gltf";
	//std::string filename = "resources\\models\\rocks\\scene.gltf";
	std::string filename = "resources\\models\\hintze_hall\\scene.gltf";

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("src\\shaders\\test.vs", "src\\shaders\\test.fs");
	Shader screenShader("src\\shaders\\screen_quad.vs", "src\\shaders\\screen_quad.fs");

	ourShader.use();
	//ourShader.setInt("texture1", 0);
	//ourShader.setInt("texture2", 1);

	if (!loadModel(modelobj, filename.c_str())) return -1;

	std::pair<std::map<int, std::map<int, GLuint>>, std::map<int, std::map<int, GLuint>>> vaoAndEbos = bindModel(modelobj);
	//dbgModel(modelobj);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	glfwSetInputMode(app.window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(app.window.window))
	{
		// per-frame time logic
		// --------------------

		// input
		// -----
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(app.window.window);
		
		ourShader.use();
		// render
		// ------
		// bind to framebuffer and draw scene as we normally would to color texture 
		glBindFramebuffer(GL_FRAMEBUFFER, app.render.framebuffer);
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
								 // make sure we clear the framebuffer's content
		glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_CULL_FACE);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		

		// bind textures on corresponding texture units
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture1);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.setVec3("light.position", cameraPos);
		ourShader.setVec3("light.direction", cameraFront);
		ourShader.setFloat("light.cutOff", glm::cos(glm::radians(22.5f)));
		ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(27.5f)));
		ourShader.setVec3("viewPos", cameraPos);

		// light properties
		ourShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
		// we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
		// each environment and lighting type requires some tweaking to get the best out of your environment.
		ourShader.setVec3("light.diffuse", 0.6f, 0.6f, 0.6f);
		ourShader.setFloat("light.constant", 1.0f);
		ourShader.setFloat("light.linear", 0.0014f);
		ourShader.setFloat("light.quadratic", 0.000007f);


		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = glm::lookAt( cameraPos,cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);		

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0, 0.0, 20.0));
		ourShader.setMat4("model", model);
		drawModel(vaoAndEbos, modelobj);

		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
								  // clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		screenShader.setFloat("iTime", app.window.iTime);
		glBindVertexArray(app.render.quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, app.render.textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		app.window.iTime++;
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(app.window.window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

#else

void line(int x0, int y0, int x1, int y1) {
	for (float t = 0.; t<1.; t += .01) {
		int x = x0 + (x1 - x0)*t;
		int y = y0 + (y1 - y0)*t;

		//(x,y) points can be defined as negative in cartesian space. The array that holds the data is 1D in only positive indices
		//this maps the negative quadrant from cartesian space to the positive position in the array. i.e. (-50, 50) in a 100x100 cartesian space
		//will get mapped to (0,0) in the array. Both are equivelant in representing lower left corner in both spaces.
		int normX = x + SCR_WIDTH/2;
		int normY = y + SCR_HEIGHT/2;
		pixels[SCR_WIDTH * normY + normX] = 1.0;
		pixels[SCR_WIDTH * normY + normX + 1] = 1.0;
		pixels[SCR_WIDTH * normY + normX + 2] = 1.0;
	}
}


void drawTriangle(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y) {
	line(p1x, p1y, p2x, p2y);
	line(p2x, p2y, p3x, p3y);
	line(p3x, p3y, p1x, p1y);
}


int main(){
	//create and init the whole application
	Application app;
	app.init();

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	Shader screenShader("src\\shaders\\raster.vs", "src\\shaders\\raster.fs");
	screenShader.use();
	//screenShader.setInt("screenTexture", 0);

	glfwSetInputMode(app.window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	meshCube.tris = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	// EAST                                                      
	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

	// NORTH                                                     
	{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

	// WEST                                                      
	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

	// TOP                                                       
	{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

	// BOTTOM                                                    
	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	};

	// Projection Matrix
	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float)SCR_HEIGHT / (float)SCR_WIDTH;
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(pixels), pixels, GL_STATIC_READ); //sizeof(data) only works for statically sized C/C++ arrays.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

	

	while (!glfwWindowShouldClose(app.window.window))
	{
		// per-frame time logic
		// --------------------

		// input
		// -----
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(app.window.window);

								 // make sure we clear the framebuffer's content
		glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
								  // clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		memset(pixels, 0, sizeof(pixels));

		// Set up rotation matrices
		mat4x4 matRotZ, matRotX;
		fTheta += 1.0f * 0.01;

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;

		// Draw Triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			// Rotate in Z-Axis
			MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

			// Rotate in X-Axis
			MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			// Offset into the screen
			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

			// Project triangles from 3D --> 2D
			MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
			MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
			MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

			// Scale into view
			//triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			//triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			//triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
			triProjected.p[0].x *= (float)SCR_WIDTH / 2;
			triProjected.p[0].y *= (float)SCR_HEIGHT / 2;
			triProjected.p[1].x *= (float)SCR_WIDTH / 2;
			triProjected.p[1].y *= (float)SCR_HEIGHT / 2;
			triProjected.p[2].x *= (float)SCR_WIDTH / 2;
			triProjected.p[2].y *= (float)SCR_HEIGHT / 2;

			// Rasterize triangle
			drawTriangle(triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y);

		}

		line(-200, -200, 200, 200);
		line(-50, -10, 50, 10);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(pixels), pixels); //to update partially
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		screenShader.use();
		glBindVertexArray(app.render.quadVAO);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		app.window.iTime++;
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(app.window.window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();


	return 0;
}
#endif


