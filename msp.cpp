// msp.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

vector<int> weight;
vector<int> parent;
vector<int> inqueue;

void swap(int &a, int &b) {
	int c = a;
	a = b;
	b = c;
}

bool Heap_Is_Empty(vector<int> &heap) {
	return heap.size() == 0;
}

void Heap_Min_Heapify(vector<int> &heap, int i) {
	int n = heap.size();
	if (i < n) {
		int l = 2 * i + 1, r = 2 * i + 2, m = i;
		if ((l < n) && (weight[heap[l]] < weight[heap[m]]))
			m = l;
		if ((r < n) && (weight[heap[r]] < weight[heap[m]]))
			m = r;
		if (m != i) {
			swap(heap[i], heap[m]);
			inqueue[heap[i]] = i;
			inqueue[heap[m]] = m;
			Heap_Min_Heapify(heap, m);
		}
	}
}

void Heap_Build_Min(vector<int> &heap) {
	for (int i = (heap.size() - 2) / 2; i >= 0; i--)
		Heap_Min_Heapify(heap, i);
}

int Heap_Extract_Min(vector<int> &heap) {
	int m = heap[0];
	inqueue[heap[0]] = -1;
	heap[0] = heap.back();
	inqueue[heap[0]] = 0;
	heap.pop_back();
	Heap_Min_Heapify(heap, 0);
	return m;
}

void Heap_Decrease_Key(vector<int> &heap, int i, int w) {
	int n = heap.size();
	if ((i < n) && (weight[heap[i]] > w)) {
		weight[heap[i]] = w;
		while ((i > 0) && (weight[heap[i]] < weight[heap[(i - 1) / 2]])) {
			swap(heap[i], heap[(i - 1) / 2]);
			inqueue[heap[i]] = i;
			inqueue[heap[(i - 1) / 2]] = (i - 1) / 2;
			i = (i - 1) / 2;
		}
	}
}

void MST_Primm(const vector<vector<int>> &G, const vector<vector<int>> &w, int r){
	int n = G.size();
	vector<int> Q(n), V;
	for (int i = 0; i < n; i++) {
		weight.push_back(INT_MAX);
		parent.push_back(-1);
		inqueue.push_back(i);
		Q[i] = i;
	}
	int v;
	weight[Q[r]] = 0;
	Heap_Build_Min(Q);
	while (!Heap_Is_Empty(Q)) {
		v = Heap_Extract_Min(Q);
		//cout << v << '\n';
		for (unsigned int i = 0; i < G[v].size(); i++) {
			if ((inqueue[G[v][i]] >= 0) && (weight[G[v][i]] > w[v][i])){
				parent[G[v][i]] = v;
				Heap_Decrease_Key(Q, inqueue[G[v][i]], w[v][i]);
			}
		}
	}
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		cout << "Invalid number of arguments\n";
		return -1;
	}
	setlocale(LC_CTYPE, "rus");

	ifstream cities;
	cities.open(argv[1]);
	int n, m;
	cities >> n;

	vector<vector<int>> neighbor(n);
	vector<vector<int>> dist(n);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++){
			cities >> m;
			if (m > 0) {
				dist[i].push_back(m);
				neighbor[i].push_back(j);
			}
			//cout << dist[i][neighbor[i].back()] << '\n';
		}
	}

	clock_t t = clock();
	MST_Primm(neighbor, dist, 0);
	t = clock() - t;

	int w = 0;
	for (int i = 0; i < n; i++)
		w += weight[i];
	cout << w << endl;
	for (int i = 0; i < n; i++)
		if (parent[i] != -1)
			cout << i << " - " << parent[i] << endl;
	cout.precision(6);
	cout << (t + 0.0) / CLOCKS_PER_SEC << endl;
	return 0;
}

