#pragma once
//#include "Framework/Graphics/Mesh.hpp"

// unused for now

//// A class that generates simple meshes for other classes to use. Creates unique outputs.
//struct GenericMeshes {
//	GenericMeshes() {}
//	/**
//	* A generic 6 vertex XYZUV mesh. (-1, -1) to (1, 1).
//	* Vertex Attributes:
//	* - 3f - xyz
//	* - 2f - uv
//	*/
//	Mesh<GLfloat> genTexcoordSquareMesh() {
//		Mesh<GLfloat> TexcoordSquare;
//		TexcoordSquare.pushVertices({
//			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // vertex 1
//			1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // vertex 2
//			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // vertex 3
//			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // vertex 4
//			1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // vertex 5
//			1.0f,  1.0f, 0.0f,  1.0f, 1.0f // vertex 6
//			});
//		TexcoordSquare.addFloatAttrib(3); // xyz
//		TexcoordSquare.addFloatAttrib(2); // uv
//		TexcoordSquare.genVBO();
//		return Mesh<GLfloat>(TexcoordSquare);
//	}
//
//};
