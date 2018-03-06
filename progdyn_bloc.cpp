#include <iostream>
#include <utility>
#include <vector>

using namespace std;

template < typename T >
using matrix4 = vector< vector< vector< vector< T > > > >;

template < typename T > using matrix2 = vector< vector< T > >;

// VARIABLES GLOBALES

int L;
int H;

// convention : les coordonnées d'un sous rectangle sont 'inclues'

// CREER STRUCT RECTANGLE
typedef struct
{
    int x1; // col sup gauche
    int y1; // ligne sup gauche
    int x2; // col inf droit
    int y2; // ligne inf droit
} Rect;

// k : commence à 1
std::pair< Rect, Rect > decoupageV(const Rect& r, int k)
{
    return std::pair< Rect, Rect >({r.x1, r.y1, r.x1 + k, r.y2},
    {r.x1 + k + 1, r.y1, r.x2, r.y2});
}

std::pair< Rect, Rect > decoupageH(const Rect& r, int k)
{
    return std::pair< Rect, Rect >({r.x1, r.y1, r.x2, r.y1 + k},
    {r.x1, r.y1 + k + 1, r.x2, r.y2});
}

int surface(const Rect& r)
{
    return (r.y2 - r.y1 + 1) * (r.x2 - r.x1 + 1);
}

// TODO : précalcul ou pas ?????????
int nb1(const matrix2< char >& pizza, const Rect& r)
{
    int nb = 0;

    for (int y = r.y1; y <= r.y2; y++)
        for (int x = r.x1; x <= r.x2; x++)
	    nb += (pizza[y][x]);

    return nb;
}

int get(const matrix4< int >& score, const Rect& r)
{
    return score[r.y1][r.x1][r.y2][r.x2];
}

int get(const matrix4< int >& score, int xinitial, int yinitial, const Rect& r)
{
    return score[r.y1-yinitial][r.x1-xinitial][r.y2-yinitial][r.x2-xinitial];
}

int set(matrix4< int >& score, const Rect& r, int value)
{
    return score[r.y1][r.x1][r.y2][r.x2] = value;
}

int set(matrix4< int >& score, int xinitial, int yinitial, const Rect& r, int value)
{
    return score[r.y1-yinitial][r.x1-xinitial][r.y2-yinitial][r.x2-xinitial] = value;
}

// L : minimum de chaque ingrédient dans la part
// H : surface maximale de la part
int optimalLocal(matrix4< int >& score, int xinitial, int yinitial, const Rect& r,
                  const matrix2< char >& pizza, int L, int H)
{
    // déjà calculé
    if (get(score, xinitial, yinitial, r) >= 0)
	return get(score, xinitial, yinitial, r);

    const int s = surface(r);
    // trop petit pour découper
    if (s <= 2 * L) {
        set(score, xinitial, yinitial, r, 0);
        return -1;
    }

    // part possible sans trou (condition de la surface <= H, #0 >= L, #1 >= L)
    int _nb1;
    if (s <= H && (_nb1 = nb1(pizza, r)) >= L && (s - _nb1) >= L)
    {
	set(score, xinitial, yinitial, r, s); // on fait une part
	return s;
    }

    // remplissage complet par heuristique
    // TODO

    // pour chaque indice de découpe VERTICAL possible
    int opt = 0;
    for (int k = 1; opt != s && k < (r.x2 - r.x1); k++)
    {
	const pair< Rect, Rect > parties = decoupageV(r, k);
	const Rect				 Gauche  = parties.first;
	const Rect				 Droite  = parties.second;

        const int local_opt_gauche = optimalLocal(score, xinitial, yinitial, Gauche, pizza, L, H);
        const int local_opt_droite = optimalLocal(score, xinitial, yinitial, Droite, pizza, L, H);
	opt = max(opt, local_opt_gauche + local_opt_droite);
    }

    // pour chaque indice de découpe HORYZONTAL possible
    for (int k = 1; opt != s && k < (r.y2 - r.y1); k++)
    {
	const pair< Rect, Rect > parties = decoupageH(r, k);
	const Rect				 Haut	= parties.first;
	const Rect				 Bas	 = parties.second;

        const int local_opt_haut = optimalLocal(score, xinitial, yinitial, Haut, pizza, L, H);
        const int local_opt_bas = optimalLocal(score, xinitial, yinitial, Bas, pizza, L, H);
	opt = max(opt, local_opt_haut + local_opt_bas);
    }

    set(score, xinitial, yinitial, r, opt);

    return opt;
}

void retrouverSolution_rec(const matrix2< char >& pizza, vector<Rect>& solution,
                           const matrix4< int >& score, int xinitial, int yinitial,
                           const Rect& r, int L, int H) {
    const int s = surface(r);
    // trop petit pour découper
    if (s <= 2 * L)
	return;

    // part possible sans trou (condition de la surface <= H, #0 >= L, #1 >= L)
    int _nb1;
    if (s <= H && (_nb1 = nb1(pizza, r)) >= L && (s - _nb1) >= L)
    {
	solution.push_back(r);
	return;
    }

    const int opt = get(score, xinitial, yinitial, r);

    // pour chaque indice de découpe VERTICAL possible
    for (int k = 1; k < (r.x2 - r.x1); k++)
    {
	const pair< Rect, Rect > parties = decoupageV(r, k);
	const Rect				 g  = parties.first;
	const Rect				 d  = parties.second;

        int local_opt_gauche = get(score, xinitial, yinitial, g);
        int local_opt_droite = get(score, xinitial, yinitial, d);

	if (opt == (local_opt_droite + local_opt_gauche))
	{
	    retrouverSolution_rec(pizza, solution, score, xinitial, yinitial, g, L, H);
	    retrouverSolution_rec(pizza, solution, score, xinitial, yinitial, d, L, H);
	    return;
	}
    }

    // pour chaque indice de découpe HORYZONTAL possible
    for (int k = 1; k < (r.y2 - r.y1); k++)
    {
	const pair< Rect, Rect > parties = decoupageH(r, k);
	const Rect				 h	= parties.first;
	const Rect				 b	 = parties.second;

        int local_opt_haut = get(score, xinitial, yinitial, h);
        int local_opt_bas = get(score, xinitial, yinitial, b);

	if (opt == (local_opt_haut + local_opt_bas))
	{
	    retrouverSolution_rec(pizza, solution, score, xinitial, yinitial, h, L, H);
	    retrouverSolution_rec(pizza, solution, score, xinitial, yinitial, b, L, H);
	    return;
	}
    }
}

vector<Rect> retrouverSolution(const matrix2< char >& pizza, const matrix4< int >& score, const Rect& bloc, int L, int H) {
    vector<Rect> solution;

    retrouverSolution_rec(pizza, solution, score, bloc.x1, bloc.y1, bloc, L, H);

    return solution;
}

void imprimer(matrix2< char >& T, Rect r, char val)
{
    for (int y = r.y1; y <= r.y2; y++)
    for (int x = r.x1; x <= r.x2; x++)
	T[y][x] = val;
}

void imprimer_solution(vector<Rect> solution, int R, int C) {
    matrix2< char > pizza(R, vector< char >(C, 0));
    int compteur = 0;
    for (Rect r : solution) {
	imprimer(pizza, r, 1+compteur%25);
	compteur++;
    }
    for (int y = 0; y < R; y++)
    {
            for (int x = 0; x < C; x++) {
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
    int B = 100;

    // récupérer l'entrée
    int R, C, L, H;
    cin >> R >> C >> L >> H;

    matrix2< char > pizza(R, vector< char >(C));

    for (int y = 0; y < R; y++)
    {
        for (int x = 0; x < C; x++)
	{
	    char c;
	    cin >> c;
	    pizza[y][x] = (c == 'T');
	}
    }

    // programmation dynamique
    // association (x1, y1, x2, y2) -> meilleur score connu sur le sous rectangle
    // (-1) indique non calculé
    matrix4< int > score = matrix4< int >(
                B, vector< vector< vector< int > > >(
                    B, vector< vector< int > >(B, vector< int >(B))));

    // Pour chaque bloc
    Rect bloc = {0, 0, B-1, B-1};

    vector<Rect> solution;

    // tant que l'on est pas complètement en dehors (en bas)
    while (bloc.y1 < R) {
	// Redimensionner
	bloc.x2 = min(bloc.x2, C-1);
	bloc.y2 = min(bloc.y2, R-1);

	// Traiter le bloc B
	//cout << "Bloc: " << bloc.x1 << ' ' << bloc.y1 << ' ' << bloc.x2 << ' ' << bloc.y2 << endl;
	for (int i1 = 0; i1 < B; i1++)
	for (int i2 = 0; i2 < B; i2++)
	for (int i3 = 0; i3 < B; i3++)
	for (int i4 = 0; i4 < B; i4++)
	    score[i1][i2][i3][i4] = -1;

	int xinital = bloc.x1;
	int yinital = bloc.y1;
	optimalLocal(score, xinital, yinital, bloc, pizza, L, H);

	const vector<Rect> solution_bloc = retrouverSolution(pizza, score, bloc, L, H);
	for (Rect r : solution_bloc)
	    solution.push_back(r);

	// Avancer (vers la droite)
	bloc.x1 += B;
	bloc.x2 += B;

	// Ca dépasse complètement (à droite)
	if (bloc.x1 >= C) {
	    bloc.x1 = 0;
	    bloc.x2 = B-1;
	    bloc.y1 += B;
	    bloc.y2 += B;
	}
    }

    //imprimer_solution(solution, R, C);
    cout << solution.size() << endl;
    for (Rect r : solution) {
	cout << r.y1 << ' ' << r.x1 << ' ' << r.y2 << ' ' << r.x2 << endl;
    }

    return 0;
}
