#include"Animaker.h"

using namespace Animaker::Core;
using namespace Animaker::Math;

Animaker::Core::GraphicsObject::GraphicsObject(){

	this->pc_gpuMatrixBuffer = nullptr;
	this->pc_gpuVertexBuffer = nullptr;
	this->pc_cpuVertexBuffer = nullptr;

}

void Animaker::Core::GraphicsObject::Interpolate(Math::Float4* from, Math::Float4* to, float t){

	Float4 temp = {};
	temp = (*from) * (1.0f - t) + (*to) * t;
	this->matrix.xyzw0.w = temp.x;
	this->matrix.xyzw1.w = temp.y;
	this->matrix.xyzw2.w = temp.z;
}

Animaker::Core::GraphicsObject::~GraphicsObject(){

	this->pc_gpuMatrixBuffer->Release();
	this->pc_gpuVertexBuffer->Release();

}
