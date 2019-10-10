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
	unordered_map<thread::id, double> tm;
	mutex lock;
	int _i, _j, cnt;
	atomic<int> __i, __j;
	queue<pair<int, int>> q;

	void funM(vector<vector<T>> & v)
	{
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

	void funA(vector<vector<T>> & v)
	{
		for (;;) {
			while (q.empty() && __i < BORDER) { this_thread::yield(); }
			if (__i >= BORDER)
				break;
			lock.lock();
			if (q.empty()) {
				lock.unlock();
				break;
			}
			auto tmp = q.front();
			q.pop();
			int l = tmp.first;
			int r = tmp.second;
			v[l][r]++;
			lock.unlock();
			this_thread::sleep_for(chrono::nanoseconds(10));
			__j++;
			if (__j >= BORDER) {
				__i++;
				__j = 0;
			}
			if (__i >= BORDER)
				break;
			q.push({ __i,__j });
		}
	}

public:
	ThreadVector(vector<vector<T>> && v, int n) : arr(move(v)), _i(0), _j(-1), __i(0), __j(0), cnt(n) { q.push({ 0,0 }); }
	ThreadVector(const vector<vector<T>> & v, int n) : arr(v), _i(0), _j(-1), __i(0), __j(0), cnt(n) { q.push({ 0,0 }); }

	void startThreadsM()
	{
		auto t = [&](vector<vector<T>> & v) { funM(v); };
		fr(cnt, i) {
			thread thr(t, ref(arr));
			threads.pb(move(thr));
		}

		fr(cnt, j)
			if (threads[j].joinable())
				threads[j].join();
	}

	void startThreadsA()
	{
		auto t = [&](vector<vector<T>> & v) { funA(v); };
		fr(cnt, i) {
			thread thr(t, ref(arr));
			threads.pb(move(thr));
		}

		fr(cnt, j)
			if (threads[j].joinable())
				threads[j].join();
	}

	void printVector()
	{
		fr(arr.size(), i) {
			fr(arr[0].size(), j)
				cout << arr[i][j];
			cout << endl;
		}
	}

	void printTime()
	{
		for (auto i = tm.begin(); i != tm.end(); ++i)
			cout << i->first << " " << i->second << endl;
	}
};

template<typename T>
class ThreadQueue {
private:
	vector<T> q;
	int p_num, t_num, c_num, ind = 0, sum = 0;
	mutex ph, p;
	condition_variable condition;
	bool done = false;
	queue<int> indexies;

public:
	ThreadQueue(int size, int t_num, int p_num, int c_num) : t_num(t_num), p_num(p_num), c_num(c_num)
	{
		q.resize(size);
	}

	void pop()
	{
		unique_lock<mutex> lock(p);
		if (!indexies.size() && done)
			return;
		if (!indexies.size())
			condition.wait(lock);
		if (!indexies.size()) // if there is waiting thread here
			return;
		auto tmp = indexies.front();
		indexies.pop();
		for (int i = tmp; i >= max(tmp - t_num + 1, 0); --i) {
			cout << "pop " << i << endl;
			sum += q[i];
		}
	}

	void push()
	{
		unique_lock<mutex> lock(ph);
		if (ind >= q.size()) {
			done = true;
			condition.notify_one();
			return;
		}
		for (int i = ind; i <= min(ind + t_num - 1, (int)q.size() - 1); ++i) {
			cout << "push " << i << endl;
			q[i]++;
		}
		indexies.push(min(ind + t_num - 1, (int)q.size() - 1));
		ind += t_num;
		condition.notify_one();
	}

	void startThreads()
	{
		vector<thread> threadsPH, threadsP;
		auto ph = [&]() { push(); };
		auto p = [&]() { pop(); };

		fr(p_num, i) {
			thread thr(ph);
			threadsPH.pb(move(thr));
		}

		fr(c_num, i) {
			thread thr(p);
			threadsP.pb(move(thr));
		}

		fr(p_num, i)
			if (threadsPH[i].joinable())
				threadsPH[i].join();
		fr(c_num, i)
			if (threadsP[i].joinable())
				threadsP[i].join();
	}

	int returnSum() {
		return sum;
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);

	//int n = BORDER, m = BORDER, k;
	//cin >> k;
	//vector<vector<int>> tmp(n, vector<int>(m));

	//ThreadVector<int> vc(move(tmp), k);
	//vc.startThreads();
	//vc.printVector();
	//vc.printTime();
	//vc.startThreadsA();
	//vc.printVector();
	//uint8_t

	ThreadQueue<uint8_t> q(4 * 1024 * 1024, 1e6, 4, 4);
	q.startThreads();
	cout << q.returnSum() << endl;

	int DEB;
	cin >> DEB;
	return 0;
}