#include <iostream>
#include <utility>
#include <vector>

using namespace std;

template < typename T >
using matrix4 = vector< vector< vector< vector< T > > > >;

template < typename T > using matrix2 = vector< vector< T > >;

// VARIABLES GLOBALES

long L;
long H;

// convention : les coordonnées d'un sous rectangle sont 'inclues'

// CREER STRUCT RECTANGLE
typedef struct
{
    long x1; // col sup gauche
    long y1; // ligne sup gauche
    long x2; // col inf droit
    long y2; // ligne inf droit
} Rect;

// k : commence à 1
std::pair< Rect, Rect > decoupageV(const Rect& r, long k)
{
    return std::pair< Rect, Rect >({r.x1, r.y1, r.x1 + k, r.y2},
    {r.x1 + k + 1, r.y1, r.x2, r.y2});
}

std::pair< Rect, Rect > decoupageH(const Rect& r, long k)
{
    return std::pair< Rect, Rect >({r.x1, r.y1, r.x2, r.y1 + k},
    {r.x1, r.y1 + k + 1, r.x2, r.y2});
}

long surface(const Rect& r)
{
    return (r.y2 - r.y1 + 1) * (r.x2 - r.x1 + 1);
}

// TODO : précalcul ou pas ?????????
long nb1(const matrix2< char >& pizza, const Rect& r)
{
    long nb = 0;

    for (long y = r.y1; y <= r.y2; y++)
	for (long x = r.x1; x <= r.x2; x++)
	    nb += (pizza[y][x]);

    return nb;
}

long get(const matrix4< long >& score, const Rect& r)
{
    return score[r.y1][r.x1][r.y2][r.x2];
}

long set(matrix4< long >& score, const Rect& r, long value)
{
    return score[r.y1][r.x1][r.y2][r.x2] = value;
}

// L : minimum de chaque ingrédient dans la part
// H : surface maximale de la part
long optimalLocal(matrix4< long >& score, const Rect& r,
		  const matrix2< char >& pizza, long L, long H)
{
    // déjà calculé
    if (get(score, r) >= 0)
	return get(score, r);

    long s = surface(r);
    // trop petit pour découper
    if (s <= 2 * L)
	return -1;

    // part possible sans trou (condition de la surface <= H, #0 >= L, #1 >= L)
    long _nb1;
    if (s <= H && (_nb1 = nb1(pizza, r)) >= L && (s - _nb1) >= L)
    {
	set(score, r, s); // on fait une part
	return s;
    }

    // remplissage complet par heuristique
    // TODO

    // pour chaque indice de découpe VERTICAL possible
    long opt = 0;
    for (long k = 1; opt != s && k < (r.x2 - r.x1); k++)
    {
	const pair< Rect, Rect > parties = decoupageV(r, k);
	const Rect				 Gauche  = parties.first;
	const Rect				 Droite  = parties.second;

	long local_opt_gauche = optimalLocal(score, Gauche, pizza, L, H);
	long local_opt_droite = optimalLocal(score, Droite, pizza, L, H);
	opt = max(opt, local_opt_gauche + local_opt_droite);
    }

    // pour chaque indice de découpe HORYZONTAL possible
    for (long k = 1; opt != s && k < (r.y2 - r.y1); k++)
    {
	const pair< Rect, Rect > parties = decoupageH(r, k);
	const Rect				 Haut	= parties.first;
	const Rect				 Bas	 = parties.second;

	long local_opt_haut = optimalLocal(score, Haut, pizza, L, H);
	long local_opt_bas = optimalLocal(score, Bas, pizza, L, H);
	opt = max(opt, local_opt_haut + local_opt_bas);
    }

    set(score, r, opt);

    return opt;
}

void retrouverSolution_rec(const matrix2< char >& pizza, vector<Rect>& solution, const matrix4< long >& score, Rect r, long L, long H) {
    long s = surface(r);
    // trop petit pour découper
    if (s <= 2 * L)
	return;

    // part possible sans trou (condition de la surface <= H, #0 >= L, #1 >= L)
    long _nb1;
    if (s <= H && (_nb1 = nb1(pizza, r)) >= L && (s - _nb1) >= L)
    {
	solution.push_back(r);
	return;
    }

    const long opt = get(score, r);

    // pour chaque indice de découpe VERTICAL possible
    for (long k = 1; k < (r.x2 - r.x1); k++)
    {
	const pair< Rect, Rect > parties = decoupageV(r, k);
	const Rect				 g  = parties.first;
	const Rect				 d  = parties.second;

	long local_opt_gauche = get(score, g);
	long local_opt_droite = get(score, d);

	if (opt == (local_opt_droite + local_opt_gauche))
	{
	    retrouverSolution_rec(pizza, solution, score, g, L, H);
	    retrouverSolution_rec(pizza, solution, score, d, L, H);
	    return;
	}
    }

    // pour chaque indice de découpe HORYZONTAL possible
    for (long k = 1; k < (r.y2 - r.y1); k++)
    {
	const pair< Rect, Rect > parties = decoupageH(r, k);
	const Rect				 h	= parties.first;
	const Rect				 b	 = parties.second;

	long local_opt_haut = get(score, h);
	long local_opt_bas = get(score, b);

	if (opt == (local_opt_haut + local_opt_bas))
	{
	    retrouverSolution_rec(pizza, solution, score, h, L, H);
	    retrouverSolution_rec(pizza, solution, score, b, L, H);
	    return;
	}
    }
}

vector<Rect> retrouverSolution(const matrix2< char >& pizza, const matrix4< long >& score, long L, long H) {
    vector<Rect> solution;

    int R = score.size();
    int C = score[0].size();

    retrouverSolution_rec(pizza, solution, score, {0, 0, C-1, R-1}, L, H);

    return solution;
}

int main()
{
    // récupérer l'entrée
    long R, C, L, H;
    cin >> R >> C >> L >> H;

    matrix2< char > pizza(R, vector< char >(C));

    for (long y = 0; y < R; y++)
    {
	for (long x = 0; x < C; x++)
	{
	    char c;
	    cin >> c;
	    pizza[y][x] = (c == 'T');
	}
    }

    // programmation dynamique
    // association (x1, y1, x2, y2) -> meilleur score connu sur le sous
    // rectangle
    // (-1) indique non calculé

    matrix4< long > score = matrix4< long >(
		R, vector< vector< vector< long > > >(
		    C, vector< vector< long > >(R, vector< long >(C, -1))));

    Rect r = {0, 0, C - 1, R - 1}; // x1 y1 x2 y2

    optimalLocal(score, r, pizza, L, H);

    vector<Rect> solution = retrouverSolution(pizza, score, L, H);
    cout << solution.size() << endl;
    for (Rect r : solution) {
	cout << r.y1 << ' ' << r.x1 << ' ' << r.y2 << ' ' << r.x2 << endl;
    }

    return 0;
}
