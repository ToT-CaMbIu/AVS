#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <stack>
#include <numeric>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <iomanip>
#include <omp.h>

using namespace std;

#define ll long long
#define ull unsigned long long
#define pb push_back
#define pp pop_back
#define mp make_pair
#define all(x) x.begin(),x.end()
#define fr(n,i) for(int i=0;i<n;++i)
#define FR(n,i) for(int i=1;i<=n;++i)
#define fr2(n,i) for(int i=0;i<n;i+=2)
#define INFINITY 1e18 + 3
#define MOD 998244353
#define BORDER 1024
const int MAX = 100005;

bool parity(int n) { return !(n & 1); }

bool is2(int n) { return n & (n - 1) == 0; }

template<typename T>
pair<T, ll> parallel(vector<T> & a, vector<T> & b) {
	auto start = chrono::high_resolution_clock::now();
	T ans = 0;
	int n = min(a.size(), b.size());
#pragma omp parallel
	{
		T local_ans = 0;
#pragma omp for schedule(static)
		fr(n, i)
			local_ans = (local_ans + a[i] * b[i]) % MOD;
#pragma omp atomic
		ans = (ans + local_ans) % MOD;
	}
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	return { ans % MOD, duration.count() };
}

template<typename T>
pair<T, ll> consistent(vector<T> & a, vector<T> & b) {
	auto start = chrono::high_resolution_clock::now();
	T ans = 0;
	int n = min(a.size(), b.size());
	fr(n, i)
		ans = (ans + a[i] * b[i]) % MOD;
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	return { ans % MOD, duration.count() };
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);

	srand(time(0));
	int n;
	cin >> n;
	vector<ll> a(n), b(n);
	fr(n, i)
		a[i] = rand(), b[i] = rand();

	auto t = parallel(a, b);
	cout << t.first << " " << t.second << endl;
	t = consistent(a, b);
	cout << t.first << " " << t.second << endl;

	int DEB;
	cin >> DEB;
	return 0;
}