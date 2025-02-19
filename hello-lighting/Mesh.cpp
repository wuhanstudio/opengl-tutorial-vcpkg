//-----------------------------------------------------------------------------
// Mesh.cpp by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Basic Mesh class
//-----------------------------------------------------------------------------
#include "Mesh.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <fmt/core.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Mesh::Mesh()
	:mLoaded(false)
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
	// Don't do this
	//glDeleteVertexArrays(1, &mVAO);
	//glDeleteBuffers(1, &mVBO);
}

void Mesh::destroy()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
}

//-----------------------------------------------------------------------------
// Loads a Wavefront OBJ model
//-----------------------------------------------------------------------------
bool Mesh::loadOBJ(const std::string& filename)
{
	std::vector<unsigned int> vertexIndices, uvIndices;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUVs;
	std::vector<glm::vec3> tempNormals;

	tinyobj::ObjReader reader;

	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = "./models"; // Path to material files

	if (!reader.ParseFromFile(filename, reader_config)) {
		if (!reader.Error().empty()) {
			fmt::println("TinyObjReader: {}", reader.Error());
		}
		return false;
	}

	if (!reader.Warning().empty()) {
		fmt::println("TinyObjReader: {}", reader.Warning());
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				Vertex meshVertex;

				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

				glm::vec3 vertex(vx, vy, vz);
				meshVertex.position = vertex;

				// Check if normal data exists
				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

					glm::vec3 normal(nx, ny, nz);
					meshVertex.normal = normal;
				}

				// Check if texcoord data exists
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

					glm::vec2 uv(tx, ty);
					meshVertex.texCoords = uv;
				}

				mVertices.push_back(meshVertex);
			}
			index_offset += fv;
		}
	}

	// Create and initialize the buffers
	initBuffers();

	return (mLoaded = true);
}

//-----------------------------------------------------------------------------
// Create and initialize the vertex buffer and vertex array object
// Must have valid, non-empty std::vector of Vertex objects.
//-----------------------------------------------------------------------------
void Mesh::initBuffers()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
}

//-----------------------------------------------------------------------------
// Render the mesh
//-----------------------------------------------------------------------------
void Mesh::draw()
{
	if (!mLoaded) return;

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	glBindVertexArray(0);
}

