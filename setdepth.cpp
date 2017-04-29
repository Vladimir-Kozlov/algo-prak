// setdepth.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <string>
#include <cmath>
#include <clocale>
#include <ctime>
using namespace std;

const double eps = 1e-6;

typedef tuple<double, double, int> point;
typedef bool(*rel)(const point&, const point&);

double dot(const point& a, const point& b) {
	return get<0>(a) * get<0>(b) +get<1>(a) * get<1>(b);
}

double vect(const point& a, const point& b) {
	return get<0>(a) * get<1>(b) - get<1>(a) * get<0>(b);
}

double norm(const point& a) {
	return sqrt(dot(a, a));
}

point operator-(const point&a, const point&b) {
	return point(get<0>(a) -get<0>(b), get<1>(a) -get<1>(b), get<2>(a));
}

point operator+(const point&a, const point&b) {
	return point(get<0>(a) +get<0>(b), get<1>(a) +get<1>(b), get<2>(a));
}

bool operator==(const point&a, const point&b) {
	return (get<0>(a) == get<0>(b)) && (get<1>(a) == get<1>(b));
}

bool operator!=(const point&a, const point&b) {
	return !(a == b);
}

bool less_polar(const point&a, const point&b) {
	double t = vect(a, b);// / (norm(a)*norm(b));
	return (t == 0) ? (norm(a) < norm(b)) : (t > 0);
}

//bool less_v(const point&a, const point&b, const point&v) {
//	double na = norm(a), nb = norm(b), nv = norm(v);
//	if (na <= eps)
//		return true;
//	if (nb <= eps)
//		return false;
//	double ca = dot(a, v) / (na * nv), cb = dot(b, v) / (nb * nv);
//	if (abs(ca - cb) <= eps)
//		return na < nb;
//	else
//		return ca > cb;
//	return false;
//}

bool less_x(const point& a, const point& b) {
	return get<0>(a) < get<0>(b);
}
bool less_y(const point& a, const point& b) {
	return get<1>(a) < get<1>(b);
}
bool eq_y(const point& a, const point& b) {
	return get<1>(a) <= get<1>(b);
}

vector<point> merge_sort(const vector<point>& data, rel less) {
	if (data.size() == 1) {
		return data;
	}
	int n = data.size();
	int p = n / 2;
	int q = n - p;
	vector<point> l(data.begin(), data.begin() + p);
	vector<point> r(data.begin() + p, data.begin() + n);
	l = merge_sort(l, less);
	r = merge_sort(r, less);
	vector<point> result(n);
	for (int i = 0, j = 0; i + j < n;) {
		if (i == p){
			result[i + j] = r[j];
			j++;
			continue;
		}
		if (j == q){
			result[i + j] = l[i];
			i++;
			continue;
		}
		if (less(l[i], r[j])){
			result[i + j] = l[i];
			i++;
		}
		else {
			result[i + j] = r[j];
			j++;
		}
	}
	return result;
}

tuple<vector<point>, vector<point>> graham_scan(vector<point> S) {
	int n = S.size();
	if (n <= 2)
		return tuple<vector<point>, vector<point>>(S, vector<point>());

	point p0 = S[0];
	int j = 0;
	for (int i = 0; i < n; i++) {
		if (less_y(S[i], p0)) {
			p0 = S[i];
			j = i;
		}
	}
	for (int i = 0; i < n; i++) {
		if (eq_y(S[i], p0) && less_x(S[i], p0)) {
			p0 = S[i];
			j = i;
		}
	}

	vector<point> T(n), R, Q, P;
	for (int i = 0; i < n; i++) T[i] = S[i] - p0;
	T = merge_sort(T, less_polar);
	R.push_back(T[0]);
	for (int i = 1; i < n - 1; i++) {
		if (vect(T[i] - T[0], T[i + 1] - T[0]) == 0)
			P.push_back(T[i]);// + p0);
		else
			R.push_back(T[i]);
	}
	R.push_back(T[n - 1]);
	if (R.size() <= 3){
		//for (unsigned int i = 0; i < R.size(); i++)
			//R[i] = R[i] + p0;
		return tuple<vector<point>, vector<point>>(R, P);
	}

	Q.push_back(R[0]);
	Q.push_back(R[1]);
	Q.push_back(R[2]);
	for (unsigned int i = 3; i < R.size(); i++) {
		while (vect(R[i] - Q[Q.size() - 1], Q[Q.size() - 1] - Q[Q.size() - 2]) >= 0) {
			P.push_back(Q.back());// + p0);
			Q.pop_back();
		}
		Q.push_back(R[i]);
	}

	//for (unsigned int i=0; i < Q.size(); i++)
		//Q[i] = Q[i] + p0;
	return tuple<vector<point>, vector<point>>(Q, P);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		cout << "Invalid number of arguments\n";
		return -1;
	}
	setlocale(LC_CTYPE, "rus");

	ifstream dots;
	dots.open(argv[1]);
	int n;
	dots >> n;

	vector<point> data(n);
	for (int i = 0; i < n; i++) {
		double x, y;
		dots >> x >> y;
		data[i] = point(x, y, i); 
	}
	dots.close();
	data = merge_sort(data, less_polar);
	vector<point> T;
	int m = 0;
	for (unsigned int i = 0; i < data.size(); i++) {
		if ((m > 0) && (data[i] == T[m-1])) {
			get<2>(T[m - 1])++;
			continue;
		}
		T.push_back(data[i]);
		m++;
		get<2>(T[m - 1]) = 1;
	}
	data = T;

	tuple<vector<point>, vector<point>> X;
	vector<int> F;
	int M = 0;
	//clock_t t;
	while (data.size() > 0) {
		//cout << data.size() << ' ';
		//t = clock();
		X = graham_scan(data);
		//cout << (clock() - t - 0.0)/CLOCKS_PER_SEC << '\n';
		M++;
		m = 0;
		for (unsigned int i = 0; i < get<0>(X).size(); i++) m += get<2>(get<0>(X)[i]);
		F.push_back(m);
		data = get<1>(X);
		//for (unsigned int i = 0; i < get<0>(X).size(); i++)
		//	cout << get<0>(get<0>(X)[i]) << ' ';
		//cout << '\n';
		//for (unsigned int i = 0; i < get<0>(X).size(); i++)
		//	cout << get<1>(get<0>(X)[i]) << ' ';
		//cout << '\n';
		//for (unsigned int i = 0; i < get<1>(X).size(); i++)
		//	cout << get<0>(get<1>(X)[i]) << ' ';
		//cout << '\n';
		//for (unsigned int i = 0; i < get<1>(X).size(); i++)
		//	cout << get<1>(get<1>(X)[i]) << ' ';
		//cout << '\n';
	}
	cout << argv[1] << '\n';
	cout << M - 1 << '\n';
	for (int m = 0; m < M; m++)
		cout << m << ", " << F[m] << '\n';
	return 0;
}

