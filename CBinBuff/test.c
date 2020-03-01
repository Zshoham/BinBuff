#include <stdio.h>
#include "binbuff.h"

typedef enum e_bool {false, true} bool;

bool test_primitive()
{
	char c = 'a';
	short s = 1;
	int i = 2;
	long l = 3;
	float f = 3.1f;
	double d = 3.2;

	bool res = true;
	status status;
	buffer *dbuf = create_dynamic_buffer(16, &status);
	buffer *sbuf = create_static_buffer(256, &status);

	write_char(dbuf, c);
	write_short(dbuf, s);
	write_int(dbuf, i);
	write_long(dbuf, l);
	write_float(dbuf, f);
	write_double(dbuf, d);

	write_char(sbuf, c);
	write_short(sbuf, s);
	write_int(sbuf, i);
	write_long(sbuf, l);
	write_float(sbuf, f);
	write_double(sbuf, d);

	char dnc, snc;
	short dns, sns;
	int dni, sni;
	long dnl, snl;
	float dnf, snf;
	double dnd, snd;

	set_mode_read(dbuf);
	set_mode_read(sbuf);

	read_char(dbuf, &dnc);
	read_short(dbuf, &dns);
	read_int(dbuf, &dni);
	read_long(dbuf, &dnl);
	read_float(dbuf, &dnf);
	read_double(dbuf, &dnd);

	read_char(sbuf, &snc);
	read_short(sbuf, &sns);
	read_int(sbuf, &sni);
	read_long(sbuf, &snl);
	read_float(sbuf, &snf);
	read_double(sbuf, &snd);

	res = res && c == dnc && c == snc;
	res = res && s == dns && s == sns;
	res = res && i == dni && i == sni;
	res = res && l == dnl && l == snl;
	res = res && f == dnf && f == snf;
	res = res && d == dnd && d == snd;

	close_buffer(&dbuf);
	close_buffer(&sbuf);

	return res;
}

bool test_primitive_array()
{
	char c[4] = { 'a', 'b', 'c', 'd' };
	short s[4] = { 1, 2 , 3, 4 };
	int i[4] = { 2, 3, 4, 5 };
	long l[4] = { 3, 4, 5, 6 };
	float f[4] = { 3.1, 3.2, 3.3, 3.4 };
	double d[4] = { 3.2, 3.3, 3.4, 3.5 };

	bool res = true;
	status status;
	buffer *dbuf = create_dynamic_buffer(16, &status);
	buffer *sbuf = create_static_buffer(256, &status);

	write_char_array(dbuf, c, 4);
	write_short_array(dbuf, s, 4);
	write_int_array(dbuf, i, 4);
	write_long_array(dbuf, l, 4);
	write_float_array(dbuf, f, 4);
	write_double_array(dbuf, d, 4);

	write_char_array(sbuf, c, 4);
	write_short_array(sbuf, s, 4);
	write_int_array(sbuf, i, 4);
	write_long_array(sbuf, l, 4);
	write_float_array(sbuf, f, 4);
	write_double_array(sbuf, d, 4);

	char dnc[4], snc[4];
	short dns[4], sns[4];
	int dni[4], sni[4];
	long dnl[4], snl[4];
	float dnf[4], snf[4];
	double dnd[4], snd[4];

	set_mode_read(dbuf);
	set_mode_read(sbuf);

	read_char_array(dbuf, dnc, 4);
	read_short_array(dbuf, dns, 4);
	read_int_array(dbuf, dni, 4);
	read_long_array(dbuf, dnl, 4);
	read_float_array(dbuf, dnf, 4);
	read_double_array(dbuf, dnd, 4);

	read_char_array(sbuf, snc, 4);
	read_short_array(sbuf, sns, 4);
	read_int_array(sbuf, sni, 4);
	read_long_array(sbuf, snl, 4);
	read_float_array(sbuf, snf, 4);
	read_double_array(sbuf, snd, 4);

	for(int j = 0; j < 4; j++)
	{
		res = res && c[j] == dnc[j] && c[j] == snc[j];
		res = res && s[j] == dns[j] && s[j] == sns[j];
		res = res && i[j] == dni[j] && i[j] == sni[j];
		res = res && l[j] == dnl[j] && l[j] == snl[j];
		res = res && f[j] == dnf[j] && f[j] == snf[j];
		res = res && d[j] == dnd[j] && d[j] == snd[j];
	}
	

	close_buffer(&dbuf);
	close_buffer(&sbuf);

	return res;
}

#pragma region GENERIC TEST

typedef  struct s_player
{
	int health;
	int attack;
	int defense;

} Player;

typedef struct s_game
{
	int width;
	int height;
	Player *player;

} Game;

bool comp_player(const Player *p1, const Player *p2)
{
	return  p1->health == p2->health && p1->attack == p2->attack && p1->defense == p2->defense;
}

bool comp_game(const Game *g1, const Game *g2)
{
	return g1->height == g2->height && g1->width == g2->width && comp_player(g1->player, g2->player);
}

status serialize_player(buffer *buffer, serializable player)
{
	Player *p = (Player *)player;
	status status = SUCCESS;
	if ((status = write_int(buffer, p->health)) != SUCCESS) return status;
	if ((status = write_int(buffer, p->attack)) != SUCCESS) return status;
	if ((status = write_int(buffer, p->defense)) != SUCCESS) return status;

	return status;
}

status deserialize_player(buffer *buffer, serializable player)
{
	Player *p = (Player *)player;
	status status = SUCCESS;
	if ((status = read_int(buffer, &p->health)) != SUCCESS) return status;
	if ((status = read_int(buffer, &p->attack)) != SUCCESS) return status;
	if ((status = read_int(buffer, &p->defense)) != SUCCESS) return status;

	return status;
}

status serialize_game(buffer *buffer, serializable game)
{
	Game *g = (Game *)game;
	status status = SUCCESS;
	if ((status = write_int(buffer, g->width)) != SUCCESS) return status;
	if ((status = write_int(buffer, g->height)) != SUCCESS) return status;
	if ((status = write_generic_data(buffer, g->player, serialize_player)) != SUCCESS) return status;

	return status;
}

status deserialize_game(buffer *buffer, serializable game)
{
	Game *g = (Game *)game;
	status status = SUCCESS;
	if ((status = read_int(buffer, &g->width)) != SUCCESS) return status;
	if ((status = read_int(buffer, &g->height)) != SUCCESS) return status;
	g->player = (Player*)malloc(sizeof(Player));
	if ((status = read_generic_data(buffer, g->player, deserialize_player)) != SUCCESS) return status;

	return status;
}

#pragma endregion GENERIC TEST

bool test_generic()
{
	Player player;
	player.health = 100;
	player.attack = 10;
	player.defense = 5;

	Game game;
	game.width = 1280;
	game.height = 720;
	game.player = &player;

	bool res = true;
	status status;
	buffer *dbuf = create_dynamic_buffer(16, &status);
	buffer *sbuf = create_static_buffer(256, &status);

	write_generic_data(dbuf, &player, serialize_player);
	write_generic_data(sbuf, &player, serialize_player);
	write_generic_data(dbuf, &game, serialize_game);
	write_generic_data(sbuf, &game, serialize_game);

	Player dnp, snp;
	Game dng, sng;

	set_mode_read(dbuf);
	set_mode_read(sbuf);

	read_generic_data(dbuf, &dnp, deserialize_player);
	read_generic_data(dbuf, &dng, deserialize_game);
	read_generic_data(sbuf, &snp, deserialize_player);
	read_generic_data(sbuf, &sng, deserialize_game);

	res = res && comp_player(&player, &dnp);
	res = res && comp_game(&game, &dng);
	res = res && comp_player(&player, &snp);
	res = res && comp_game(&game, &sng);


	return res;
}

bool test_generic_array()
{
	Player *players[5];
	Player ps[5];
	for (int i = 0; i < 5; i++)
	{
		ps[i].health = 100 + i;
		ps[i].attack = 10 + i;
		ps[i].defense = 5 + i;
		players[i] = &ps[i];
	}
	

	Game *games[5];
	Game gs[5];
	for (int i = 0; i < 5; ++i)
	{
		gs[i].width = 1280;
		gs[i].height = 720;
		gs[i].player = players[i];
		games[i] = &gs[i];
	}
	

	bool res = true;
	status status;
	buffer *dbuf = create_dynamic_buffer(16, &status);
	buffer *sbuf = create_static_buffer(4096, &status);

	write_generic_data_array(dbuf, (serializable*)players, 5, serialize_player);
	write_generic_data_array(sbuf, (serializable*)players, 5, serialize_player);
	write_generic_data_array(dbuf, (serializable*)games, 5, serialize_game);
	write_generic_data_array(sbuf, (serializable*)games, 5, serialize_game);

	Player *dnp[5], *snp[5];
	Game *dng[5], *sng[5];

	for (int i = 0; i < 5; ++i)
	{
		dnp[i] = (Player*)malloc(sizeof(Player));
		dng[i] = (Game*)malloc(sizeof(Game));
		snp[i] = (Player*)malloc(sizeof(Player));
		sng[i] = (Game*)malloc(sizeof(Game));
	}

	set_mode_read(dbuf);
	set_mode_read(sbuf);

	read_generic_data_array(dbuf, (serializable*)dnp, 5, deserialize_player);
	read_generic_data_array(dbuf, (serializable*)dng, 5, deserialize_game);
	read_generic_data_array(sbuf, (serializable*)snp, 5, deserialize_player);
	read_generic_data_array(sbuf, (serializable*)sng, 5 ,deserialize_game);

	for(int i = 0; i < 5; i++)
	{
		res = res && comp_player(players[i], dnp[i]);
		res = res && comp_game(games[i], dng[i]);
		res = res && comp_player(players[i], snp[i]);
		res = res && comp_game(games[i], sng[i]);
	}

	return res;
}


int main(int argc, char* argv[])
{
	if (!test_primitive()) puts("failed primitive serialization test.");
	else if (!test_primitive_array()) puts("failed primitive array serialization test.");
	else if (!test_generic()) puts("failed generic serialization test.");
	else if (!test_generic_array()) puts("failed generic array serialization test.");
	else puts("all tests passed.");
	
	char finish = getchar();

	return 0;
}
