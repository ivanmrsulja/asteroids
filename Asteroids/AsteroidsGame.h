#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Sprites.h"
#include <list>
#include <algorithm>
#include <string>

#define GAME_SPEED 5
#define BULLET_SIZE 1
#define BULLET_SPEED 20
#define DIFFICULTY_CHANGE_INTERVAL 5
#define SCREEN_HEIGHT 270
#define SCREEN_WIDTH 480

// Difficulty represents 1:x chance for an asteroid to be spawned every frame 
int difficulty = 500;
int last_change = 0;

// Texture and models loading
std::string path1 = "..\\icons\\asteroiddd.png";
std::string path2 = "..\\icons\\background.png";
olc::Sprite* ast = new olc::Sprite(path1);
olc::Sprite* bkg = new olc::Sprite(path2);


class Game : public olc::PixelGameEngine
{
public:
	int ship_x_position;
	int ship_y_position;
	std::list<Asteroid> asteroids;
	std::list<Bullet> projectiles;
	int score;
	bool game_over;

	Game() :
		ship_x_position(SCREEN_WIDTH / 2),
		ship_y_position(SCREEN_HEIGHT / 2),
		score(0),
		game_over(false)
	{
		sAppName = "Asteroids";
	}

	bool OnUserCreate() override
	{
		
		// Pixel drawing optimisation provided by the engine
		SetPixelMode(olc::Pixel::MASK);

		// Called once at the start, so create things here
		DrawCircle(ship_x_position, ship_y_position, 5, olc::Pixel(olc::WHITE));

		// Added so i could test collisions and some other things, unnecessary but useful
		Asteroid first(100, 100, 10, LINEAR, 5, ship_x_position, ship_y_position);
		asteroids.push_back(first);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		// Used only in GAME OVER screen
		olc::HWButton key_enter = GetKey(olc::Key::ENTER);
		if (key_enter.bPressed)
		{
			game_over = false;
			asteroids.clear();
			projectiles.clear();
			score = 0;
			difficulty = 500;
			OnUserCreate();
		}

		if (game_over)
		{
			return true;
		}

		// Left click triggers projectile spawn
		olc::HWButton key_mouse = GetMouse(0);

		if (key_mouse.bPressed)
		{
			Bullet novi(GetMouseX(), GetMouseY(), ship_x_position, ship_y_position);
			projectiles.push_back(novi);
		}

		if (score % DIFFICULTY_CHANGE_INTERVAL == 0 && difficulty > 549 && score != last_change)
		{
			difficulty -= 50;
			last_change = score;
			std::cout << "Trenutna tezina: " << difficulty << std::endl;
		}

		// Simple math for pseudo-randomizing asteroid spawn locations
		int spawn_chance = rand() % difficulty;
		int direction_x = 1, direction_y = 1;
		int speed = 5;
		Type_of_movement movement = LINEAR;

		if (spawn_chance == 1)
		{
			if (rand() % 2 == 1) { direction_x = -1; }
			if (rand() % 2 == 1) { direction_y = -1; }
			if (rand() % 3 == 1) { movement = PARABOLIC; speed = 2; }
			Asteroid novi(((rand() % 100) + 150)* direction_x, ((rand() % 100) + 150)*direction_y, 10, movement, speed, ship_x_position, ship_y_position);
			asteroids.push_back(novi);
			
		}

		// Draw background and stationary objects, update score (if needed)
		DrawSprite(0, 0, bkg, 1);
		DrawCircle(ship_x_position, ship_y_position, 5, olc::Pixel(olc::WHITE));
		DrawString(0, 0, "SCORE:" + std::to_string(score), olc::Pixel(olc::WHITE), 1);

		//calculate new positions and redraw all asteroids
		calculate_asteroid_positions(fElapsedTime);

		for (Asteroid& a : asteroids)
		{
			if (sqrt(pow(a.x_pos - ship_x_position, 2) + pow(a.y_pos - ship_y_position, 2)) <= a.radius + 5)
			{
				DrawString(130, 100, "GAME OVER!", olc::Pixel(olc::RED), 3);
				DrawString(140, 160, "Press ENTER to try again...", olc::Pixel(olc::RED), 1);
				game_over = true;
			}

			DrawSprite(a.x_pos - ast->width / 2, a.y_pos - ast->height / 2, ast, 0.3);
			//DrawCircle(a.x_pos, a.y_pos, a.radius, olc::Pixel(olc::BLUE));   //uncomment to see hitboxes
			//DrawLine(a.start_x , a.start_y , a.final_x , a.final_y );        //uncomment to see path
		}

		//calculate new positions and redraw all projectiles
		calculate_projectile_positions(fElapsedTime);

		for (Bullet& b : projectiles)
		{
			DrawCircle(b.x_pos, b.y_pos, BULLET_SIZE, olc::Pixel(olc::RED));
		}

		if (projectiles.size() != 0)
		{
			asteroids.remove_if([&](const Asteroid& a) {return collides(a); });
			projectiles.remove_if([&](const Bullet& b) {return out_of_bounds(b); });
		}

		return true;
	}


private:
	void calculate_asteroid_positions(float time) {
		for (Asteroid& as : asteroids) {
			as.x_pos += (time * GAME_SPEED)*as.speed * as.direction;
			switch (as.movement) {
			case LINEAR:
				lagrange_linear_interpolation(as);
				break;
			case PARABOLIC:
				lagrange_quad_interpolation(as);
				break;
			}
		}
	}

	void calculate_projectile_positions(float time) {
		for (Bullet& b : projectiles) {
			double angle = abs(ship_y_position - b.final_y) / 2;
			double x1 = b.x_pos;
			b.x_pos += (time * GAME_SPEED) * BULLET_SPEED * b.direction;
			double y1 = b.y_pos;
			lagrange_linear_interpolation(b);

			if (abs(b.y_pos - y1) > 1.3) {
				//std::cout << abs(b.y_pos - y1) << std::endl;
				b.x_pos = x1 + (time * GAME_SPEED) * (BULLET_SPEED / (abs(b.y_pos - y1))) * b.direction;
				lagrange_linear_interpolation(b);
			}
		}
	}


	bool collides(const Asteroid& a) {
		for (const Bullet& b : projectiles) {

			double distance = sqrt(pow(a.x_pos - b.x_pos, 2) + pow(a.y_pos - b.y_pos, 2));

			if (distance <= (double)(a.radius + BULLET_SIZE))
			{
				projectiles.remove(b);
				score += 1;
				return true;
			}
		}
		return false;
	}

	bool out_of_bounds(const Bullet& b) {
		if (b.y_pos < 500 && b.y_pos > 0 && b.x_pos > 0 && b.x_pos < 500) { return false; }
		return true;
	}

	template<typename T>
	void lagrange_linear_interpolation(T& a) {
		double y = (a.start_y * ((a.x_pos - a.final_x) / (a.start_x - a.final_x))) + (a.final_y * ((a.x_pos - a.start_x) / (a.final_x - a.start_x)));
		a.y_pos = y;
	}

	void lagrange_quad_interpolation(Asteroid& a) {
		std::cout << ship_x_position << std::endl;
		double y = (a.start_y * (((a.x_pos - ship_x_position) * (a.x_pos - a.final_x)) / ((a.start_x - ship_x_position) * (a.start_x - a.final_x)))) * (ship_y_position * (((a.x_pos - a.start_x) * (a.x_pos - a.final_x)) / ((ship_x_position - a.start_x) * (ship_x_position - a.final_x)))) * (a.final_y * (((a.x_pos - a.start_x) * (a.x_pos - ship_x_position)) / ((a.final_x - a.start_x) * (a.final_x - ship_x_position))));
		std::cout << y << " " << a.x_pos << std::endl;
		a.y_pos = y + 240;
		//a.x_pos = 240;
	}
};