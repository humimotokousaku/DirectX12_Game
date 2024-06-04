#include "Collider.h"

bool Collider::TriggerOnCollision() {
	if (!isPreOnCollision_ && isOnCollision_) {
		return true;
	}
	else {
		return false;
	}
}

bool Collider::ReleaseOnCollision() {
	if (isPreOnCollision_ && !isOnCollision_) {
		return true;
	}
	else {
		return false;
	}
}

bool Collider::PressOnCollision() {
	if (isOnCollision_) {
		return true;
	}
	else {
		return false;
	}
}