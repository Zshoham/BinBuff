#include "Player.h"

Player::Player(int player_num) : player_num(player_num)
{
	this->health = 100 * player_num;
	this->isAlive = true;
	this->strength = 10 * player_num;
}

bool Player::operator==(const Player& other) const
{
	return this->player_num == other.player_num && this->health == other.health
		&& this->isAlive == other.isAlive && this->strength == other.strength;
}

bool Player::operator==(const Player* other) const
{
	return this->player_num == other->player_num && this->health == other->health
		&& this->isAlive == other->isAlive && this->strength == other->strength;
}
