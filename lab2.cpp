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
	vector<T> arr_atomic;
	vector<thread> threads;
	unordered_map<thread::id, double> tm;
	mutex lock;
	int _i, cnt;
	atomic<int> __i = 0;

	void funM(vector<vector<T>> & v)
	{
		for (;;) {
			lock.lock();
			time_t start, end;
			time(&start);
			_i++;
			if (_i >= BORDER * BORDER) {
				lock.unlock();
				break;
			}
			arr_atomic[_i]++;
			this_thread::sleep_for(chrono::nanoseconds(10));
			time(&end);
			tm[this_thread::get_id()] += (double)(difftime(end, start));
			lock.unlock();
		}
	}

	void funA(vector<T> & v) {
		for (;;) {
			auto tmp = __i.fetch_add(1, memory_order_relaxed);
			if (tmp >= BORDER * BORDER)
				return;
			v[tmp]++;
			this_thread::sleep_for(chrono::nanoseconds(10));
		}
	}

public:
	ThreadVector(vector<vector<T>> && v, int n) : arr(move(v)), _i(-1), __i(0), cnt(n) { }
	ThreadVector(const vector<vector<T>> & v, int n) : arr(v), _i(-1), __i(0), cnt(n) { }
	ThreadVector(vector<T> && v, int n) : arr_atomic(move(v)), _i(-1), __i(0), cnt(n) { }
	ThreadVector(const vector<T> & v, int n) : arr_atomic(v), _i(-1), __i(0), cnt(n) {  }

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

	void startThreadsALinear()
	{
		auto t = [&](vector<T> & v) { funA(v); };
		fr(cnt, i) {
			thread thr(t, ref(arr_atomic));
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

	void printLinear() {
		fr(arr_atomic.size(), i)
			cout << arr_atomic[i] << " ";
		cout << endl;
	}

	void printTime()
	{
		for (auto i = tm.begin(); i != tm.end(); ++i)
			cout << i->first << " " << i->second << endl;
	}
};

template<typename T>
class ThreadQueueM {
private:
	vector<int> q;
	int p_num, t_num, c_num, size, ind = 0, ind1 = 0;
	mutex ph, p, lockph, lockp;
	condition_variable conditionPH, conditionP;
	atomic<int> cur = 0;
	int sum = 0;
	bool flg;

public:
	ThreadQueueM(int size, int t_num, int p_num, int c_num) : t_num(t_num), p_num(p_num), c_num(c_num), size(size)
	{
		q.resize(size);
		fr(q.size(), i)
			q[i] = 0;
		flg = false;
	}

	void pop()
	{
		lockp.lock();
		for (int i = (ind1 % size), j = 0; j < t_num; ++i, ++j) {
			unique_lock<mutex> lock(p);
			while (cur.load() <= 0) {
				conditionP.wait(lock);
			}
			if (!q[i % size])
				this_thread::sleep_for(chrono::milliseconds(1));
			sum += q[i % size];
			q[i % size] = 0;
			cur--;
			conditionPH.notify_all();
		}
		ind1 += t_num;
		ind1 %= t_num;
		lockp.unlock();
	}

	void push()
	{
		lockph.lock();
		for (int i = (ind % size), j = 0; j < t_num; ++i, ++j) {
			unique_lock<mutex> lock(p);
			while (cur.load() >= size) {
				conditionPH.wait(lock);
			}
			if (q[i % size])
				this_thread::sleep_for(chrono::milliseconds(1));
			q[i % size] = 1;
			cur++;
			conditionP.notify_all();
		}
		ind += t_num;
		ind %= t_num;
		lockph.unlock();
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

template<typename T>
class ThreadQueueA
{
private:
	vector<T> q;
	int p_num, t_num, c_num, size;
	atomic<int> sum, curSizePH = 0, curSizeP = 0;
	mutex lock, lock1, locktest1;
	atomic<T> ind1 = 0, ind2 = 0;

public:
	ThreadQueueA(int size, int t_num, int p_num, int c_num) : t_num(t_num), p_num(p_num), c_num(c_num), size(size)
	{
		q.resize(size);
	}

	void pop()
	{
		for (;;) {
			{
				unique_lock<mutex> un(lock1);
				if (curSizeP.load() >= c_num * t_num)
					return;
			}
			T i = ind2.load(memory_order_relaxed);
			while (!ind2.compare_exchange_weak(
				i,
				(ind2 + 1) % q.size(),
				std::memory_order_release,
				std::memory_order_relaxed));
			{
				unique_lock<mutex> un(locktest1);
				if (!q[(i % q.size())])
					continue;
				sum += q[(i % q.size())];
				q[(i % q.size())] = 0;
			}
			curSizeP++;
		}
	}

	void push()
	{
		for (;;) {
			{
				unique_lock<mutex> un(lock);
				if (curSizePH.load() >= p_num * t_num)
					return;
			}
			T i = ind1.load(memory_order_relaxed);
			while (!ind1.compare_exchange_weak(
				i,
				(ind1 + 1) % q.size(),
				std::memory_order_release,
				std::memory_order_relaxed));
			{
				unique_lock<mutex> un(locktest1);
				if (q[(i % q.size())])
					continue;
				q[(i % q.size())]++;
			}
			curSizePH++;
		}
	}

	void print() {
		int ans = 0;
		fr(q.size(), i)
			cout << q[i] << " ", ans += q[i];
		cout << endl;
		cout << ans << endl;
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

template<typename T>
class ThreadQueue
{
private:
	queue<T> q;
	mutex lock1, lock2, lock3;
	int p_num, t_num, c_num, i = 0, j = 0;
	ll sum = 0;

public:
	ThreadQueue(int t_num, int p_num, int c_num) : t_num(t_num), p_num(p_num), c_num(c_num) {}

	void pop()
	{
		lock2.lock();
		while (i >= j * t_num) {
			if (j >= p_num && q.empty()) {
				lock2.unlock();
				return;
			}
		}
		fr(t_num, k) {
			lock3.lock();
			sum += q.front();
			q.pop();
			lock3.unlock();
		}
		i++;
		lock2.unlock();
	}

	void push()
	{
		lock1.lock();
		fr(t_num, k) {
			lock3.lock();
			q.push(1);
			lock3.unlock();
		}
		j++;
		lock1.unlock();
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

	ll returnSum() {
		return sum;
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);

	int DEBUG = 3;

	if (DEBUG == 1) {
		int n = BORDER, m = BORDER, k;
		cin >> k;
		vector<vector<int>> tmp(n, vector<int>(m));
		vector<int> tm(n * n);

		ThreadVector<int> vc(move(tm), k); // <1 for atomic and <10 for lock
		vc.startThreadsM();
		//vc.startThreadsALinear();
		//vc.printLinear();
		vc.printTime();
	}

	if (DEBUG == 2) {
		ThreadQueueM<uint8_t> q(4, 1024 * 1024, 4, 4); //1.10
		q.startThreads();
		cout << q.returnSum() << endl;
	}

	if (DEBUG == 3) {
		ThreadQueue<uint8_t> q(4 * 1024 * 1024, 4, 4);//1.20
		q.startThreads();
		cout << q.returnSum() << endl;
	}

	if (DEBUG == 4) {
		ThreadQueueA<int> q(16, 1024 * 1024 * 4, 2, 2);//1.37
		q.startThreads();
		cout << q.returnSum() << endl;
	}

	int DEB;
	cin >> DEB;
	return 0;
}