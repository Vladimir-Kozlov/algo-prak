// mindist.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <string>
using namespace std;

typedef tuple<double, double, int> point;
typedef bool(*rel)(const point&, const point&);

bool less_x(const point& a, const point& b) {
	return get<0>(a) < get<0>(b);
}

bool less_y(const point& a, const point& b) {
	return get<1>(a) < get<1>(b);
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

double dist(const point& a, const point& b) {
	return sqrt((get<0>(a) -get<0>(b)) * (get<0>(a) -get<0>(b)) +
		(get<1>(a) -get<1>(b)) * (get<1>(a) -get<1>(b)));
}

tuple<point, point, double> min_dist(const vector<point>& X, const vector<point>& Y) {
	tuple<point, point, double> result;
	double d, d_opt;
	if (X.size() <= 3) {
		for (unsigned int i = 0; i < X.size() - 1; i++) 
			for (unsigned int j = i + 1; j < X.size(); j++) {
				d = dist(X[i], X[j]);
				if (i == 0 && j == 1 || d < d_opt) {
					d_opt = d;
					result = tuple<point, point, double>(X[i], X[j], d_opt);
				}
			}
		return result;
	}
	int n = X.size();
	int p = n / 2;
	int q = n - p;
	vector<point>
		XL(X.begin(), X.begin() + p),
		XR(X.begin() + p, X.begin() + n),
		YL(p), YR(q), YC;
	for (int i = 0, j = 0; i + j < n;){
		if (i == p){
			YR[j] = Y[i + j];
			j++;
			continue;
		}
		if (j == q){
			YL[i] = Y[i + j];
			i++;
			continue;
		}
		if (get<0>(Y[i + j]) >= get<0>(X[p])){
			YR[j] = Y[i + j];
			j++;
		}
		else {
			YL[i] = Y[i + j];
			i++;
		}
	}
	tuple<point, point, double>
		dl = min_dist(XL, YL),
		dr = min_dist(XR, YR);
	result = (get<2>(dl) < get<2>(dr)) ? dl : dr;
	d_opt = get<2>(result);
	for (int i = 0; i < n; i++)
		if (get<0>(X[p]) - d_opt <= get<0>(Y[i]) && get<0>(X[p]) + d_opt >= get<0>(Y[i]))
			YC.push_back(Y[i]);
	for (unsigned int i = 0; i < YC.size(); i++)
		for (unsigned int j = i + 1; j < i + 8 && j < YC.size(); j++){
			d = dist(YC[i], YC[j]);
			if (d < d_opt){
				d_opt = d;
				result = tuple<point, point, double>(YC[i], YC[j], d);
			}
		}
	return result;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		cout << "Invalid number of arguments\n";
		return -1;
	}

	ifstream dots;
	dots.open(argv[1]);
	int n;
	dots >> n;
	vector<tuple<double, double, int>> data(n);
	for (int i = 0; i < n; i++) {
		double x, y;
		dots >> x >> y;
		data[i] = tuple<double, double, int>(x, y, i); //cout << x << ' ' << y << ' ' << i << '\n';
	}
	dots.close();
	vector<tuple<double, double, int>> 
		X = merge_sort(data, less_x), 
		Y = merge_sort(data, less_y);
	tuple<point, point, double> result = min_dist(X, Y);
	string t(argv[1]);
	unsigned int t1 = t.rfind('\\'),
		t2 = t.rfind('/');
	unsigned int t0 = (t1 < t2) ? t1 : t2;
	if (t0 == string::npos)
		cout << argv[1] << '\n';
	else
		cout << t.substr(t0 + 1) << '\n';
	cout << get<2>(get<0>(result)) + 1 << ' ' << get<2>(get<1>(result)) + 1 << '\n';
	cout << '(' << get<0>(get<0>(result)) << ", " << get<1>(get<0>(result)) << "), " <<
		'(' << get<0>(get<1>(result)) << ", " << get<1>(get<1>(result)) << ")\n";
	cout << get<2>(result);
	return 0;
}

