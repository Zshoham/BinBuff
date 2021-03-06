#include <gtest/gtest.h>

#include <binbuff.h>
#include "Game.h"
#include "Player.h"
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

using namespace binbuff;

TEST(BinbuffTest, Primitive)
{
	char c = 'a';
	bool b = true;
	short s = 1;
	int i = 2;
	long l = 3;
	float f = 3.1f;
	double d = 3.2;

	Buffer dbuf(Buffer::DYNAMIC);
	Buffer sbuf(Buffer::STATIC, 64);

	dbuf << c << b << s << i << l << f << d;

	sbuf << c << b << s << i << l << f << d;

	char dnc, snc;
	bool dnb, snb;
	short dns, sns;
	int dni, sni;
	long dnl, snl;
	float dnf, snf;
	double dnd, snd;

	dbuf.set_mode_read();
	sbuf.set_mode_read();

	dbuf >> dnc >> dnb >> dns >> dni >> dnl >> dnf >> dnd;

	sbuf >> snc >> snb >> sns >> sni >> snl >> snf >> snd;

	EXPECT_EQ(c, dnc);
	EXPECT_EQ(c, snc);

	EXPECT_EQ(b, dnb);
	EXPECT_EQ(b, snb);

	EXPECT_EQ(s, dns);
	EXPECT_EQ(s, sns);

	EXPECT_EQ(i, dni);
	EXPECT_EQ(i, sni);

	EXPECT_EQ(l, dnl);
	EXPECT_EQ(l, snl);

	EXPECT_EQ(f, dnf);
	EXPECT_EQ(f, snf);

	EXPECT_EQ(d, dnd);
	EXPECT_EQ(d, snd);
}

TEST(BinbuffTest, PrimitiveArray)
{
	char c[4] = { 'a', 'b', 'c', 'd' };
	bool b[4] = { true, true, false, true };
	short s[4] = { 1, 2 , 3, 4 };
	int i[4] = { 2, 3, 4, 5 };
	long l[4] = { 3, 4, 5, 6 };
	float f[4] = { 3.1f, 3.2f, 3.3f, 3.4f };
	double d[4] = { 3.2, 3.3, 3.4, 3.5 };

	Buffer dbuf(Buffer::DYNAMIC);
	Buffer sbuf(Buffer::STATIC, 128);

	dbuf.write(c, 4);
	dbuf.write(b, 4);
	dbuf.write(s, 4);
	dbuf.write(i, 4);
	dbuf.write(l, 4);
	dbuf.write(f, 4);
	dbuf.write(d, 4);

	sbuf.write(c, 4);
	sbuf.write(b, 4);
	sbuf.write(s, 4);
	sbuf.write(i, 4);
	sbuf.write(l, 4);
	sbuf.write(f, 4);
	sbuf.write(d, 4);

	char dnc[4], snc[4];
	bool dnb[4], snb[4];
	short dns[4], sns[4];
	int dni[4], sni[4];
	long dnl[4], snl[4];
	float dnf[4], snf[4];
	double dnd[4], snd[4];

	dbuf.set_mode_read();
	sbuf.set_mode_read();

	dbuf.read(dnc, 4);
	dbuf.read(dnb, 4);
	dbuf.read(dns, 4);
	dbuf.read(dni, 4);
	dbuf.read(dnl, 4);
	dbuf.read(dnf, 4);
	dbuf.read(dnd, 4);

	sbuf.read(snc, 4);
	sbuf.read(snb, 4);
	sbuf.read(sns, 4);
	sbuf.read(sni, 4);
	sbuf.read(snl, 4);
	sbuf.read(snf, 4);
	sbuf.read(snd, 4);

	for (int j = 0; j < 4; j++)
	{
		EXPECT_EQ(c[j], dnc[j]);
		EXPECT_EQ(c[j], snc[j]);

		EXPECT_EQ(b[j], dnb[j]);
		EXPECT_EQ(b[j], snb[j]);

		EXPECT_EQ(s[j], dns[j]);
		EXPECT_EQ(s[j], sns[j]);

		EXPECT_EQ(i[j], dni[j]);
		EXPECT_EQ(i[j], sni[j]);

		EXPECT_EQ(l[j], dnl[j]);
		EXPECT_EQ(l[j], snl[j]);

		EXPECT_EQ(f[j], dnf[j]);
		EXPECT_EQ(f[j], snf[j]);

		EXPECT_EQ(d[j], dnd[j]);
		EXPECT_EQ(d[j], snd[j]);
	}
}

TEST(BinbuffTest, Generic)
{
	Player player(5);
	Game game(720, 1280, 2);

	Buffer dbuf(Buffer::DYNAMIC);
	Buffer sbuf(Buffer::STATIC, 1024);

	dbuf << player << game;
	sbuf << player << game;

	auto serializer = [](Buffer &buffer, const Player &player) { buffer << player.player_num; };

	dbuf.write<Player>(player, serializer);
	sbuf.write<Player>(player, serializer);

	Player dnp, snp, dlnp, slnp;
	Game dng, sng;

	dbuf.set_mode_read();
	sbuf.set_mode_read();

	dbuf >> dnp >> dng;
	sbuf >> snp >> sng;

	auto deserializer = [](Buffer &buffer, Player &player) { buffer >> player.player_num; };

	dbuf.read<Player>(dlnp, deserializer);
	sbuf.read<Player>(slnp, deserializer);

	EXPECT_EQ(player, dnp);
	EXPECT_EQ(player, snp);

	EXPECT_EQ(game, dng);
	EXPECT_EQ(game, sng);

	EXPECT_EQ(player.player_num, dlnp.player_num);
	EXPECT_EQ(player.player_num, slnp.player_num);
}

TEST(BinbuffTest, GenericArray)
{
	Player players[2];
	players[0] = Player(1);
	players[1] = Player(2);
	Game games[2];
	games[0] = Game(1280, 720, 1);
	games[1] = Game(1280, 720, 2);

	Buffer dbuf(Buffer::DYNAMIC);
	Buffer sbuf(Buffer::STATIC, 1024);

	dbuf.write(players, 2);
	dbuf.write(games, 2);
	sbuf.write(players, 2);
	sbuf.write(games, 2);

	Player dnp[2], snp[2];
	Game dng[2], sng[2];

	dbuf.set_mode_read();
	sbuf.set_mode_read();

	dbuf.read(dnp, 2);
	dbuf.read(dng, 2);
	sbuf.read(snp, 2);
	sbuf.read(sng, 2);

	for (int i = 0; i < 2; ++i)
	{
		EXPECT_EQ(players[i], dnp[i]);
		EXPECT_EQ(players[i], snp[i]);
		EXPECT_EQ(games[i], dng[i]);
		EXPECT_EQ(games[i], sng[i]);
	}
}

TEST(BinbuffTest, Containers)
{
	struct hash_player
	{
		size_t operator()(const Player &player) const
		{
			std::hash<int> hasher;
			return hasher(player.player_num);
		}
	};

	Player p1(1);
	Player p2(2);

	Game g1(1280, 720, 1);
	Game g2(1280, 720, 2);

	std::shared_ptr<Player> sp = std::make_shared<Player>(1);
	std::pair<Player, Game> pgp = std::make_pair(p1, g1);

	std::array<Player, 2> arr;
	arr[0] = p1;
	arr[1] = p2;

	std::vector<std::shared_ptr<Game>> vec;
	vec.push_back(std::make_shared<Game>(g1));
	vec.push_back(std::make_shared<Game>(g2));

	std::deque<Player> deque;
	deque.push_back(p1);
	deque.push_back(p2);

	std::forward_list<Game> flist;
	flist.push_front(g1);
	flist.push_front(g2);

	std::list<Player *> list;
	list.push_back(&p1);
	list.push_back(&p1);

	std::set<Game> set;
	set.insert(g1);
	set.insert(g2);

	std::unordered_set<Player, hash_player> uset;
	uset.insert(p1);
	uset.insert(p2);

	std::unordered_map<int, Game> umap;
	umap.insert(std::make_pair(1, g1));
	umap.insert(std::make_pair(2, g2));

	std::map<int, Player> map;
	map[1] = p1;
	map[2] = p2;

	std::stack<Game *> stack;
	stack.push(&g1);
	stack.push(&g2);

	std::queue<Player> queue;
	queue.push(p1);
	queue.push(p2);

	Buffer buf(Buffer::DYNAMIC);

	buf << sp << pgp;
	buf << arr << vec << deque << flist << list;
	buf << set << map << uset << umap;
	buf << stack << queue;

	buf.set_mode_read();

	std::shared_ptr<Player> nsp = std::make_shared<Player>();
	std::pair<Player, Game> npgp;

	std::array<Player, 2> narr;
	std::vector<std::shared_ptr<Game>> nvec;
	std::deque<Player> ndeque;
	std::forward_list<Game> nflist;
	std::list<std::shared_ptr<Player>> nlist;
	std::set<Game> nset;
	std::unordered_set<Player, hash_player> nuset;
	std::unordered_map<int, Game> numap;
	std::map<int, Player> nmap;
	std::stack<std::shared_ptr<Game>> nstack;
	std::queue<Player> nqueue;

	buf >> nsp >> npgp >> narr;
	buf.read(nvec, 2);
	buf.read(ndeque, 2);
	buf.read(nflist, 2);
	buf.read(nlist, 2);
	buf.read(nset, 2);
	buf.read(nmap, 2);
	buf.read(nuset, 2);
	buf.read(numap, 2);
	buf.read(nstack, 2);
	buf.read(nqueue, 2);

	EXPECT_EQ(*nsp, *sp);
	EXPECT_EQ(npgp, pgp);
	EXPECT_EQ(narr, arr);
	EXPECT_EQ(ndeque, deque);
	EXPECT_EQ(nflist, flist);
	EXPECT_EQ(nset, set);
	EXPECT_EQ(nmap, map);
	EXPECT_EQ(nuset, uset);
	EXPECT_EQ(numap, umap);

	for (int i = 0; i < 2; i++)
	{
		EXPECT_EQ(*nlist.front(), *list.front());
		nlist.pop_front();
		list.pop_front();
		EXPECT_EQ(*(nvec[i]), *(vec[i]));
	}
}