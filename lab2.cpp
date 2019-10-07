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
#define BORDER 20
const int MAX = 100005;

bool parity(int n) { return !(n & 1); }

bool is2(int n) { return n & (n - 1) == 0; }

template<typename T>
class ThreadVector {
private:
	vector<vector<T>> arr;
	vector<thread> threads;
	mutex lock;
	int _i, _j, cnt;

	void fun(vector<vector<T>> & v) {
		for (;;) {
			{
				//lock_guard<std::mutex> lock(lock);
				lock.lock();
				if (_i >= BORDER) {
					lock.unlock();
					break;
				}
				v[_i][_j]++;
				_j++;
				if (_j == BORDER) {
					_i++;
					_j = 0;
				}
			}
			lock.unlock();
		}
	}

public:
	ThreadVector(vector<vector<T>> && v, int n) : arr(move(v)), _i(0), _j(0), cnt(n) {}
	ThreadVector(const vector<vector<T>> & v, int n) : arr(v), _i(0), _j(0), cnt(n) {}

	void startThreads() {
		auto t = [&](vector<vector<T>> & v) { fun(v); };
		fr(cnt, i) {
			thread thr(t, ref(arr));
			threads.pb(move(thr));
		}

		fr(cnt, j)
			if (threads[j].joinable())
				threads[j].join();
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

	int n = BORDER, m = BORDER, k;
	cin >> k;
	vector<vector<int>> tmp(n, vector<int>(m));

	ThreadVector<int> vc(move(tmp), k);

	vc.startThreads();
	vc.printVector();

	int DEB;
	cin >> DEB;
	return 0;
}