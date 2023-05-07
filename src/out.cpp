// File compiled by CPCC
// Built by competitive programmers. For competitive programmers. https://github.com/kevlu8/CPCC

#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define ld long double;

const int INF = 2147483647;
const ll LLINF = 9223372036854775807;

ll factorial(ll x, ll mod=LLINF) {
	ll res = 1;
	for (ll i = 2; i <= x; i++) res = (res*i) % mod;
	return res;
}

struct SegTree {
	int size=1;
	vector<int> a;
	void init(int n) {
		while (size<n) size *= 2;
		a.resize(size*2);
	}
	void update(int i, ll x) {
		i += size; // subtract 1 if index is 1-indexed
		a[i] = x;
		while (i > 1) {
			i /= 2;
			a[i] = a[i*2] + a[i*2+1];
		}
	}
	ll query(int l, int r) {
		l += size; r += size;
		ll res = 0;
		while (l <= r) {
			if (l % 2 == 0) res += a[l++];
			if (r % 2 == 1) res += a[r--];
			l /= 2;
			r /= 2;
		}
		return res;
	}
}

