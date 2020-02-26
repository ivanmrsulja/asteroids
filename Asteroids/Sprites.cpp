#pragma once
#include "Sprites.h"
#include <iostream>

Asteroid::Asteroid(double x, double y, double r, Type_of_movement t, double s, double ship_x, double ship_y) : start_x(x), start_y(y), radius(r), movement(t), speed(s)
{
	if (start_x < 0) {
		direction = 1;
	}
	else {
		direction = -1;
	}

	switch (movement)
	{
	case LINEAR:
		final_x = -x + ship_x;
		final_y = -y + ship_y;
		start_x += ship_x;
		start_y += ship_y;
		x_pos = start_x;
		y_pos = start_y;
		break;
	case PARABOLIC:
		final_x = -x*23 + ship_x;
		final_y = start_y + ship_y;
		start_x += ship_x;
		start_y += ship_y;
		x_pos = start_x;
		y_pos = start_y;
		//std::cout << "X: " << start_x << " Y: " << start_y << " FinX: " << final_x << " FinY: " << final_y << std::endl;  used for debugging
		break;
	}
}

Asteroid::~Asteroid() {}

bool Asteroid::operator==(const Asteroid& a) {
	if (x_pos == a.x_pos && y_pos == a.y_pos) {
		return true;
	}
	return false;
}


Bullet::Bullet(double x, double y, double ship_x, double ship_y) : final_x(x), final_y(y), x_pos(ship_x), y_pos(ship_y), start_x(ship_x), start_y(ship_y) {
	if (final_x > ship_x) {
		direction = 1;
	}
	else {
		direction = -1;
	}
}

bool Bullet::operator==(const Bullet& b) {
	if (x_pos == b.x_pos && y_pos == b.y_pos) {
		return true;
	}
	return false;
}

Bullet::~Bullet() {}