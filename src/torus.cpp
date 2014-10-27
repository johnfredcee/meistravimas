
#include <SDL.h>
#include <SDL_opengl.h>
#include <memory>
#include "tuple.h"
#include "twodee.h"
#include "threedee.h"
#include "bufferbuilder.h"
#include "torus.h"

namespace venk
{

/**
 * radius is the radius of the torus as a whole
 * minorRadius is the radius of the circle the torus sweeps
 */
TorusBuilder::TorusBuilder(float radius, float minorRadius, Uint32 numSeg, Uint32 numSegMinor)
{

	vertexBuffer = std::make_shared<BufferBuilder>(GL_FLOAT, 3);
	normalBuffer = std::make_shared<BufferBuilder>(GL_FLOAT, 3);
	uvBuffer     = std::make_shared<BufferBuilder>(GL_FLOAT, 2);
	indexBuffer  = std::make_shared<BufferBuilder>(GL_UNSIGNED_SHORT, 1);


	float theta = 0.0f;
	float delta_theta = 2.0f * M_PI / numSeg;

	for(Uint32 i = 0; i < numSeg; i++) {
		float xi = cosf(theta) * radius;
		float yi = sinf(theta) * radius;
		float phi = 0.0f;
		float delta_phi = 2.0f * M_PI / numSegMinor;

		for(Uint32 j = 0; j < numSegMinor; j++) {
			float x = xi + cosf(theta) * minorRadius * cosf(phi);
			float y = yi + sinf(theta) * minorRadius * cosf(phi);
			float z = minorRadius * sinf(phi);
			vertexBuffer->addVec3f(x, y, z);
			Vector3d normal(x - xi, y - yi, z);
			normal.normalise();
			normalBuffer->addVec3f(normal.x(), normal.y(), normal.z());
			Vector2d uv(j / (float) numSegMinor, i / (float) numSeg);
			uvBuffer->addVec2f(uv.u(), uv.v());
			phi += delta_phi;
		}
		theta += delta_theta;
	}
	for(Uint32 i = 1; i < numSeg; i++) {
		for(Uint32 j = 1; j < numSegMinor; j++) {
			Uint16 i00 = (i-1) * numSegMinor + (j - 1);
			Uint16 i01 = (i-1) * numSegMinor + j;
			Uint16 i10 = i * numSegMinor + (j - 1);
			Uint16 i11 = i * numSegMinor + j;
			indexBuffer->addVec1ui(i00);
			indexBuffer->addVec1ui(i01);
			indexBuffer->addVec1ui(i10);
			indexBuffer->addVec1ui(i01);
			indexBuffer->addVec1ui(i11);
			indexBuffer->addVec1ui(i10);
		}
		Uint16 i00 = (i-1) * numSegMinor + (numSegMinor - 1);
		Uint16 i01 = (i-1) * numSegMinor;
		Uint16 i10 = i * numSegMinor + (numSegMinor - 1);
		Uint16 i11 = i * numSegMinor;
		indexBuffer->addVec1ui(i00);
		indexBuffer->addVec1ui(i01);
		indexBuffer->addVec1ui(i10);
		indexBuffer->addVec1ui(i01);
		indexBuffer->addVec1ui(i11);
		indexBuffer->addVec1ui(i10);
	}
	for(Uint32 j = 1; j < numSegMinor; j++) {
		Uint16 i00 = (numSeg-1) * numSegMinor + (j - 1);
		Uint16 i01 = (numSeg-1) * numSegMinor + j;
		Uint16 i10 = (j - 1);
		Uint16 i11 = j;
		indexBuffer->addVec1ui(i00);
		indexBuffer->addVec1ui(i01);
		indexBuffer->addVec1ui(i10);
		indexBuffer->addVec1ui(i01);
		indexBuffer->addVec1ui(i11);
		indexBuffer->addVec1ui(i10);
	}
	Uint16 i00 = (numSeg-1) * numSegMinor + (numSegMinor - 1);
	Uint16 i01 = (numSeg-1) * numSegMinor;
	Uint16 i10 = 0;
	Uint16 i11 = 1;
	indexBuffer->addVec1ui(i00);
	indexBuffer->addVec1ui(i01);
	indexBuffer->addVec1ui(i10);
	indexBuffer->addVec1ui(i01);
	indexBuffer->addVec1ui(i11);
	indexBuffer->addVec1ui(i10);
	return;
}

std::shared_ptr<Buffer> TorusBuilder::make_vertices() {
	return vertexBuffer->make_buffer(GL_ARRAY_BUFFER);
}

std::shared_ptr<Buffer> TorusBuilder::make_indices() {
	return indexBuffer->make_buffer(GL_ELEMENT_ARRAY_BUFFER);
}

}
