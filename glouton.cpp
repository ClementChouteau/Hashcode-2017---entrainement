#include <algorithm>
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

typedef struct Rect
{
	long x1; // col sup gauche
	long y1; // ligne sup gauche
	long x2; // col inf droit
	long y2; // ligne inf droit

	/*
	bool operator==(const Rect& other) const
	{
		return !(x1 == other.x1 && y1 == other.y1 && x2 == x2 &&
				 y2 == other.y2);
	}
	*/
} Rect;

namespace std
{
template <> struct hash< Rect >
{
	size_t operator()(const Rect& r) const noexcept
	{
		return std::hash< int >{}(
			(int)(abs(r.x2 - r.x1) * 31 + abs(r.y2 - r.y1)));
	}
};
}

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

long nb1(const matrix2< int >& sommes_ligne, const Rect& r)
{
	int somme = 0;
	for (int y = r.y1; y <= r.y2; y++)
	{
		somme += sommes_ligne[y][r.x2 + 1] - sommes_ligne[y][r.x1];
	}

	return somme;
}

long get_r(const matrix4< long >& score, const Rect& r)
{
	return score[r.y1][r.x1][r.y2][r.x2];
}

long set_r(matrix4< long >& score, const Rect& r, long value)
{
	return score[r.y1][r.x1][r.y2][r.x2] = value;
}

void imprimer(matrix2< char >& T, Rect r, char val)
{
    for (int y = r.y1; y <= r.y2; y++)
    for (int x = r.x1; x <= r.x2; x++)
	T[y][x] = val;
}

// void calculer_sous_rectangles_possible(unordered_set< Rect >& traites,
// 									   const Rect& r, long L, long H)
// {
// 	// rectangle possible
// 	if (2 * L <= surface(r) && surface(r) <= H)
// 	{
// 		// rectangle pas traité
// 		if (traites.find(r) == traites.end())
// 		{
// 			traites.insert(r);
// 			calculer_sous_rectangles_possible(
// 				traites, {r.x1, r.y1, r.x2 - 1, r.y2}, L, H);
// 			calculer_sous_rectangles_possible(
// 				traites, {r.x1, r.y1, r.x2 - 1, r.y2 - 1}, L, H);
// 		}
// 	}
// }

vector< Rect > calculer_rectangles_possible(long L, long H)
{
	vector< Rect > rectangles_possible;

	for (int y2 = 0; y2 < H; y2++)
		for (int x2 = 0; x2 < H; x2++)
		{
			Rect r = {0, 0, x2, y2};
			if (2 * L <= surface(r) && surface(r) <= H)
				rectangles_possible.push_back(r);
		}

	sort(rectangles_possible.begin(), rectangles_possible.end(),
		 [](const Rect& lhs, const Rect& rhs) {
			 if (surface(lhs) > surface(rhs))
				 return true;
			 else
				 return lhs.x2 >= rhs.x2;
		 });
	return rectangles_possible;
}

vector< Rect > glouton(const matrix2< char >& pizza, Rect r, long L, long H)
{
	vector< Rect > resultat;

	long R = pizza.size();
	long C = pizza[0].size();

	long Y = r.y2 - r.y1 + 1;
	long X = r.x2 - r.x1 + 1;

	// précalcul
	matrix2< int > sommes_ligne(
		R, vector< int >(C + 1, 0)); // première somme {} = 0

	for (int y = 0; y < R; y++)
	{
		long somme = 0;
		for (int x = 0; x < C; x++)
		{
			somme += (pizza[y][x]);
			sommes_ligne[y][1 + x] = somme;
		}
	}

	vector< Rect > rectangles_possible = calculer_rectangles_possible(L, H);

	// algorithme glouton
	matrix2< char > utilise(
		Y, vector< char >(X, 0)); // impression des rectangles déjà utilisés

	for (int y = r.y1; y <= r.y2; y++)
	{
		for (int x = r.x1; x <= r.x2; x++)
		{
			if (utilise[y-r.y1][x-r.x1])
				continue;

			// Pour chaque taille de rectangle possible (du plus grand au plus
			// petit
			for (const Rect& r_forme : rectangles_possible)
			{
				Rect r_glouton;
				r_glouton.x1 = x;
				r_glouton.y1 = y;
				r_glouton.x2 = x + r_forme.x2;
				r_glouton.y2 = y + r_forme.y2;

				// vérifier taille (rentre dans le sous rectangle)
				if (r_glouton.x2 > r.x2)
					continue;
				if (r_glouton.y2 > r.y2)
					continue;

				// vérifier contraintes problème
				int n1 = nb1(sommes_ligne, r_glouton);
				int n0 = surface(r_glouton) - n1;
				if (n1 < L || n0 < L)
					continue;

				// vérifier non intersection
				for (int _y = r_glouton.y1; _y <= r_glouton.y2; _y++)
					for (int _x = r_glouton.x1; _x <= r_glouton.x2; _x++)
						if (utilise[_y - r.y1][_x - r.x1])
							goto prochain_rect;

				// On choisit ce rectangle
				resultat.push_back(r_glouton);

				imprimer(utilise, {r_glouton.x1 - r.x1,
						   r_glouton.y1 - r.y1,
						   r_glouton.x2 - r.x1,
						   r_glouton.y2 - r.y1},
					 1);

				/*
				cout << r_glouton.x1 << ' ' << r_glouton.y1 << ' ' << r_glouton.x2 << ' ' << r_glouton.y2 << endl;
				for (int y = 0; y < Y; y++) {
					for (int x = 0; x <= X; x++)
					{
					    if (utilise[y][x] == 0)
						cout << '0';
					    else
						cout << '1';
					}
					cout << endl;
				}
				cout << endl;
				*/

				break;
			prochain_rect:;
			}
		}
	}

	return resultat;
}

void imprimer_solution(vector<Rect> solution, int R, int C) {
    matrix2< char > pizza(R, vector< char >(C, 0));
    int compteur = 0;
    for (Rect r : solution) {
	imprimer(pizza, r, 1+compteur%25);
	compteur++;
    }
    for (long y = 0; y < R; y++)
    {
	    for (long x = 0; x < C; x++) {
		if (pizza[y][x] == 0)
		    cout << '0';
		else
		    cout << (char)('a' + pizza[y][x]);
	    }

	    cout << endl;
    }
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

	Rect tout = {0, 0, C - 1, R - 1}; // x1 y1 x2 y2
	vector< Rect > solution = glouton(pizza, tout, L, H);

	matrix2< char > res(R, vector< char >(C));

	cout << solution.size() << endl;
	for (const Rect& r : solution)
	{
		cout << r.y1 << ' ' << r.x1 << ' ' << r.y2 << ' ' << r.x2 << endl;
	}

	//cout << endl;
	//imprimer_solution(solution, R, C);

	return 0;
}
