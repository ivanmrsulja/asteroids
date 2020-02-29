#pragma once

enum Type_of_movement { PARABOLIC, LINEAR };

struct Asteroid {

	double x_pos;
	double y_pos;
	double radius;
	double start_x;
	double start_y;
	double final_x;
	double final_y;
	double speed;
	int direction;
	bool danger;
	Type_of_movement movement;

	Asteroid(double x, double y, double r, Type_of_movement t, double s, double ship_x, double ship_y);

	~Asteroid();

	bool operator==(const Asteroid& a);
};

struct Bullet {

	double x_pos;
	double y_pos;
	double start_x;
	double start_y;
	double final_x;
	double final_y;
	int direction;

	Bullet(double x, double y, double ship_x, double ship_y);

	bool operator==(const Bullet& b);

	~Bullet();

};