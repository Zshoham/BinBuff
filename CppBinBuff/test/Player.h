#pragma once

#include "binbuff.h"


class Player : public bbf::Serializable
{
	bool isAlive;
    double health;
	int strength;

public:

	int player_num;

	explicit Player(int player_num);
	Player() : isAlive(false), health(0), strength(0), player_num(0) {}

	bool operator==(const Player& other) const;
	bool operator==(const Player *other) const;

	bool operator<(const Player& other) const { return this->player_num < other.player_num; }


	void serialize(bbf::Buffer& buffer) const override { buffer << this->player_num << this->isAlive << this->health << this->strength; }
	void deserialize(bbf::Buffer& buffer) override { buffer >> this->player_num >> this->isAlive >> this->health >> this->strength; }
};

