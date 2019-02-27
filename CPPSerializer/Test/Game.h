#ifndef GAME_H
#define GAME_H


#include "CPPSerializer.h"
#include "Player.h"

class Game : public ser::Serializable
{
	int width, height;
	Player *player;
	int num_enemies;
	Player **enemies;

public:
	Game(int width, int height, int num_enemies);
	Game() : width(0), height(0), player(nullptr), num_enemies(0), enemies(nullptr)  {}
	Game(const Game& other);
	~Game();

	Game& operator=(const Game& other);
	Game& operator=(Game&& other) noexcept;


	bool operator==(const Game& other) const;
	bool operator!=(const Game& other) const { return !(*this == other); }

	bool operator<(const Game& other) const { return this->num_enemies < other.num_enemies; }

	void serialize(ser::Buffer& buffer) const override;
	void deserialize(ser::Buffer& buffer) override;
};

#endif
