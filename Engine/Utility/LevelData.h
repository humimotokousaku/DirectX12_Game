#pragma once
#include "ModelStructs.h"
#include <vector>

class LevelData {
public:
	struct ObjectData {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
		std::string fileName;
	};
	struct CurveData {
		std::string type;
		std::vector<Vector3> points;
	};
	std::vector<Transform> transform_;
	std::vector<std::string> type_;
	std::vector<std::string> name_;

	std::vector<ObjectData> objects_;

	std::vector<CurveData> railCameraControlPoint_;
};