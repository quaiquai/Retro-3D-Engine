#define _CRT_SECURE_NO_WARNINGS//need for tinygltf

//uncomment when using tinygltf
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <glad/glad.h>
#include "tiny_gltf.h"
#include <iostream>

tinygltf::Model modelobj;

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) //used for tinygltf

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

			}
			else if (vaa == 3) {//tangents
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