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

using namespace std;

typedef long double ld;
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
const int MAX = 100005;

bool parity(int n) { return !(n & 1); }

bool is2(int n) { return n & (n - 1) == 0; }

template<typename T>
class ThreadVector {
private:
	vector<vector<T>> arr;
	ll cnt;

	void fun(vector<vector<T>> & v) {
		for (int i = 0; i < v[cnt % v.size()].size(); ++i)
			v[cnt % v.size()][i]++;
	}

public:
	ThreadVector(vector<vector<T>> && v) : arr(move(v)), cnt(0) {}
	ThreadVector(const vector<vector<T>> & v) : arr(v), cnt(0) {}

	void startThread() {
		auto t = [&](vector<vector<T>> & v) { fun(v); }; //try to make without lambda
		thread thr(t,ref(arr));
		thr.join();
		cnt++;
	}

	void printVector() {
		fr(arr.size(), i) {
			fr(arr[0].size(), j)
				cout << arr[i][j] << " ";
			cout << endl;
		}
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);

	int n, m, k;
	cin >> n >> m >> k;
	vector <vector<int>> tmp(n, vector<int>(m));
	fr(n, i)
		fr(m, j)
			tmp[i][j] = j;

	ThreadVector<int> vc(move(tmp));

	fr(k,i)
		vc.startThread();

	vc.printVector();

	int DEB;
	cin >> DEB; //thks vs
	return 0;
}
