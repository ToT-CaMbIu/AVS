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
class ThreadVector {
private:
	vector<vector<T>> arr;
	vector<thread> threads;
	unordered_map<thread::id,double> tm;
	mutex lock;
	int _i, _j, cnt;
	atomic<int> __i, __j;
	queue<pair<int, int>> q;

	void funM(vector<vector<T>> & v) {
		for (;;) {
			lock.lock();
			time_t start, end;
			time(&start);
			_j++;
			if (_j == BORDER) {
				_i++;
				_j = 0;
			}
			if (_i >= BORDER) {
				lock.unlock();
				break;
			}
			v[_i][_j]++;
			this_thread::sleep_for(chrono::nanoseconds(10));
			time(&end);
			tm[this_thread::get_id()] += (double)(difftime(end, start));
			lock.unlock();
		}
	}

	void funA(vector<vector<T>> & v) {
		for (;;) {
			int l = 0, r = 0;
			lock.lock();
			while (q.empty() && __i < BORDER) { this_thread::yield(); }
			if (__i >= BORDER) {
				lock.unlock();
				break;
			}
			auto tmp = q.front();
			q.pop();
			l = tmp.first;
			r = tmp.second;
			if (l < BORDER && r < BORDER) {
				v[l][r]++;
				this_thread::sleep_for(chrono::nanoseconds(10));
			}
			lock.unlock();
			__j++;
			if (__j.load() >= BORDER) {
				__i++;
				__j = 0;
			}
			if (__i.load() >= BORDER)
				break;
			q.push({ __i,__j });
		}
	}

public:
	ThreadVector(vector<vector<T>> && v, int n) : arr(move(v)), _i(0), _j(-1), __i(0), __j(0), cnt(n) { q.push({ 0,0 }); }
	ThreadVector(const vector<vector<T>> & v, int n) : arr(v), _i(0), _j(-1), __i(0), __j(0), cnt(n) { q.push({ 0,0 }); }

	void startThreadsM() {
		auto t = [&](vector<vector<T>> & v) { funM(v); };
		fr(cnt, i) {
			thread thr(t, ref(arr));
			threads.pb(move(thr));
		}

		fr(cnt, j)
			if (threads[j].joinable())
				threads[j].join();
	}

	void startThreadsA() {
		auto t = [&](vector<vector<T>> & v) { funA(v); };
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
				cout << arr[i][j];
			cout << endl;
		}
	}

	void printTime() {
		for (auto i = tm.begin(); i != tm.end(); ++i)
			cout << i->first << " " << i->second << endl;
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
	//vc.startThreads();
	//vc.printVector();
	//vc.printTime();
	vc.startThreadsA();
	vc.printVector();

	int DEB;
	cin >> DEB;
	return 0;
}