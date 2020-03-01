#include "Game.h"

Game::Game(int width, int height, int num_enemies) : width(width), height(height), num_enemies(num_enemies)
{
	this->player = new Player(1);
	this->enemies = new Player*[num_enemies];
	for (int i = 0; i < num_enemies; ++i)
	{
		this->enemies[i] = new Player(i + 2);
	}
}

Game::Game(const Game& other)
{
	this->player = new Player;
	*this->player = *other.player;
	this->enemies = new Player*[other.num_enemies];
	for (int i = 0; i < other.num_enemies; ++i)
	{
		this->enemies[i] = new Player(*other.enemies[i]);
	}
	this->num_enemies = other.num_enemies;
	this->height = other.height;
	this->width = other.width;
}

Game::~Game()
{
	delete this->player;
	for (int i = 0; i < this->num_enemies; ++i)
	{
		delete this->enemies[i];
	}
	delete[] this->enemies;
}

Game& Game::operator=(const Game& other)
{
	if (this->player == nullptr) this->player = new Player;
	*this->player = *other.player;
	for (int i = 0; i < this->num_enemies; ++i)
	{
		delete this->enemies[i];
	}
	delete[] this->enemies;
	this->enemies = new Player*[other.num_enemies];
	for (int i = 0; i < other.num_enemies; ++i)
	{
		this->enemies[i] = new Player;
		*this->enemies[i] = *other.enemies[i];
	}
	this->num_enemies = other.num_enemies;
	this->height = other.height;
	this->width = other.width;

	return *this;
}

Game& Game::operator=(Game&& other) noexcept
{
	delete this->player;
	for (int i = 0; i < this->num_enemies; ++i)
	{
		delete this->enemies[i];
	}
	delete[] this->enemies;

	this->player = other.player;
	this->enemies = other.enemies;
	this->height = other.height;
	this->num_enemies = other.num_enemies;
	this->width = other.width;

	other.player = nullptr;
	other.enemies = nullptr;
	other.height = 0;
	other.num_enemies = 0;
	other.width = 0;

	return *this;
}

bool Game::operator==(const Game& other) const
{
	bool areEqual = this->height == other.height && this->width == other.width;
	if (!areEqual) return false;
	for (int i = 0; i < this->num_enemies; ++i)
	{
		areEqual = areEqual && *this->enemies[i] == other.enemies[i];
	}
	return areEqual && *this->player == other.player;
}

void Game::serialize(bbf::Buffer& buffer) const
{
	buffer << this->width << this->height;
	buffer << *this->player;
	buffer << this->num_enemies;
	buffer.write(this->enemies, this->num_enemies);
}

void Game::deserialize(bbf::Buffer& buffer)
{
	buffer >> this->width >> this->height;
	this->player = new Player;
	buffer >> *this->player;
	buffer >> this->num_enemies;
	this->enemies = new Player*[this->num_enemies];
	buffer.read(this->enemies, this->num_enemies); 
}
