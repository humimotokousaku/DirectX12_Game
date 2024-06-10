#include "Cube.h"
#include "PipelineManager.h"

Cube::Cube() {
	Initialize();
}
Cube::~Cube() {

}

void Cube::Initialize() {
	IPrimitive::Initialize();
}

void Cube::Draw(uint32_t textureHandle) {
	PipelineManager::GetInstance()->SetSkyboxPSO();
	IPrimitive::Draw(textureHandle);
}