#include "CPPSerializer.h"
#include "Game.h"
#include "Player.h"
#include <iostream>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

using namespace ser;

bool test_primitive()
{
	
	char c = 'a';
	bool b = true;
	short s = 1;
	int i = 2;
	long l = 3;
	float f = 3.1f;
	double d = 3.2;

	bool res = true;
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

	res = res && c == dnc && c == snc;
	res = res && b == dnb && b == snb;
	res = res && s == dns && s == sns;
	res = res && i == dni && i == sni;
	res = res && l == dnl && l == snl;
	res = res && f == dnf && f == snf;
	res = res && d == dnd && d == snd;

	return res;
}

bool test_primitive_array()
{
	char c[4] = { 'a', 'b', 'c', 'd' };
	bool b[4] = { true, true, false, true };
	short s[4] = { 1, 2 , 3, 4 };
	int i[4] = { 2, 3, 4, 5 };
	long l[4] = { 3, 4, 5, 6 };
	float f[4] = { 3.1f, 3.2f, 3.3f, 3.4f };
	double d[4] = { 3.2, 3.3, 3.4, 3.5 };

	bool res = true;
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
		res = res && c[j] == dnc[j] && c[j] == snc[j];
		res = res && b[j] == dnb[j] && b[j] == snb[j];
		res = res && s[j] == dns[j] && s[j] == sns[j];
		res = res && i[j] == dni[j] && i[j] == sni[j];
		res = res && l[j] == dnl[j] && l[j] == snl[j];
		res = res && f[j] == dnf[j] && f[j] == snf[j];
		res = res && d[j] == dnd[j] && d[j] == snd[j];
	}

	return res;
}

bool test_generic()
{
	Player player(5);
	Game game(720, 1280, 2);

	bool res = true;
	Buffer dbuf(Buffer::DYNAMIC);
	Buffer sbuf(Buffer::STATIC, 1024);

	dbuf << player << game;
	sbuf << player << game;
	
	Player dnp, snp;
	Game dng, sng;
	
	dbuf.set_mode_read();
	sbuf.set_mode_read();

	dbuf >> dnp >> dng;
	sbuf >> snp >> sng;
	
	res = res && player == dnp && player == snp;
	res = res && game == dng && game == sng;
	
	return res;
}

bool test_generic_array()
{
	Player players[2];
	players[0] = Player(1);
	players[1] = Player(2);
	Game games[2];
	games[0] = Game(1280, 720, 1);
	games[1] = Game(1280, 720, 2);


	bool res = true;
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
		res = res && players[i] == dnp[i];
		res = res && players[i] == snp[i];
		res = res && games[i] == dng[i];
		res = res && games[i] == sng[i];
	}

	return res;
}

struct hash_player
{
	size_t operator()(const Player& player) const
	{
		std::hash<int> hasher;
		return hasher(player.player_num);
	}
};

bool test_containers()
{
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
	
	std::list<Player*> list;
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
	
	std::stack<Game*> stack;
	stack.push(&g1);
	stack.push(&g2);
	
	std::queue<Player> queue;
	queue.push(p1);
	queue.push(p2);	

	bool res = true;

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
	std::list<Player*> nlist;
	std::set<Game> nset;
	std::unordered_set<Player, hash_player> nuset;
	std::unordered_map<int, Game> numap;
	std::map<int, Player> nmap;
	std::stack<Game*> nstack;
	std::queue<Player> nqueue;

	buf.read(nsp);
	buf.read(npgp);
	buf.read(narr);
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

	res = res && *nsp == *sp;
	res = res && npgp == pgp;
	res = res && narr == arr;
	res = res && ndeque == deque;
	res = res && nflist == flist;
	res = res && nset == set && nmap == map && nuset == uset && numap == umap;

	for (int i = 0; i < 2; i++)
	{
		res = res && *nlist.front() == *list.front();
		nlist.pop_front();
		list.pop_front();
		res = res && *(nvec[i]) == *(vec[i]);
	}

	return res;
}

int main(int argc, char* argv[])
{
	if (!test_primitive()) std::cout << "failed primitive serialization test." << std::endl;
	else if (!test_primitive_array()) std::cout << "failed primitive array serialization test." << std::endl;
	else if (!test_generic()) std::cout << "failed generic serialization test." << std::endl;
	else if (!test_generic_array()) std::cout << "failed generic array serialization test." << std::endl;
	else if (!test_containers()) std::cout << "failed container serialization test." << std::endl;
	else std::cout << "all tests passed." << std::endl;

	std::cin.get();

	return 0;
}
