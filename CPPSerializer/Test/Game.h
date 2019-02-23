#ifndef GAME_H
#define GAME_H


#include "CPPSerializer.h"
#include "Player.h"

class Game : public Serializable
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

	void serialize(Buffer& buffer) const override;
	void deserialize(Buffer& buffer) override;
};

#endif
