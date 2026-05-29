#include <bits/stdc++.h>
using namespace std;

#define all(x) x.begin(), x.end()
using ll = long long;
using pll = array<ll, 2>;
using ar4 = array<ll, 4>;

struct network {
	const ll logmaxflow = 30;
	vector<ar4>eg;
	vector<vector<ll>>g;
	vector<ll>e, d;
	vector<ll>p;
	ll n;
	network(ll _n) :n(_n), g(_n), e(_n), p(_n), d(_n) {};
	void add(ll u, ll v, ll f, ll c) {
		if (f == 0)
			return;
		g[u].push_back(eg.size());
		eg.push_back({ u, v, f, c });
		g[v].push_back(eg.size());
		eg.push_back({ v, u, 0, -c });
	}

	void bfs(ll start, ll b) {
		fill(all(d), -1);
		d[start] = 0;
		queue<ll>q;
		q.push(start);
		while (!q.empty()) {
			ll u = q.front();
			q.pop();
			for (ll id : g[u]) {
				ll v = eg[id][1];
				if (eg[id][2] < b) continue;
				if (d[v] == -1) d[v] = d[u] + 1, q.push(v);
			}
		}
	}

	ll dfs(ll u, ll finish, ll b, ll flow) {
		if (u == finish) return flow;
		for (; e[u] < g[u].size(); ++e[u]) {
			ll id = g[u][e[u]];
			ll v = eg[id][1];
			if (eg[id][2] < b || d[v] != d[u] + 1) continue;
			ll upd = dfs(v, finish, b, min(eg[id][2], flow));
			if (upd) {
				eg[id][2] -= upd;
				eg[id ^ 1][2] += upd;
				return upd;
			}
		}
		return 0;
	}

	ll dinic(ll start, ll finish, ll b) {
		ll ans = 0;
		while (1) {
			bfs(start, b);
			if (d[finish] == -1) break;
			fill(all(e), 0);

			while (1) {
				ll f = dfs(start, finish, b, 1 << logmaxflow);
				if (f) ans += f;
				else break;
			}
		}
		return ans;
	}

	ll maxflow(ll start, ll finish) {
		ll ans = 0;
		for (ll pw = logmaxflow; pw >= 0; --pw) {
			ans += dinic(start, finish, (1 << pw));
		}
		return ans;
	}


	pll mincostmaxflow(ll start, ll finish) {
		ll flow = maxflow(start, finish);

		fill(all(p), 0);
		fill(all(e), 0);

		ll eps = 1ll << 50;

		for (auto& [u, v, f, c] : eg)
			c *= n * 4;

		while (eps > 1) {
			eps = eps / 2;
			excess(eps);

		}
		ll ans = 0;
		for (ll i = 1; i < eg.size(); i += 2)
			ans += eg[i][2] * -eg[i][3];
		return { flow, ans / (n * 4) };
	}
	ll cost(ll id) { return p[eg[id][0]] - p[eg[id][1]] + eg[id][3]; }
	void excess(ll eps) {
		//priority_queue<pll>q;
		queue<pll>q;
		ll id = 0;

		for (auto [u, v, f, w] : eg) {
			if (f > 0 && cost(id) < eps) {
				e[u] -= eg[id][2];
				e[v] += eg[id][2];
				if (e[v] > 0)
					q.push({ -p[v],v });
				eg[id ^ 1][2] += eg[id][2];
				eg[id][2] = 0;
			}
			++id;
		}

		while (q.size()) {
			//auto [w, u] = q.top();
			auto [w, u] = q.front();
			q.pop();
			if (e[u] <= 0 || p[u] != -w)
				continue;
			for (ll id : g[u]) {
				if (cost(id) < 0 && eg[id][2] > 0) {
					ll v = eg[id][1];
					ll f = min(e[u], eg[id][2]);
					eg[id][2] -= f;
					eg[id ^ 1][2] += f;
					e[u] -= f;
					e[v] += f;
					if (e[v] > 0)
						q.push({ -p[v], v });
				}
			}
			if (e[u] > 0) {
				ll mn = 1e18;
				for (ll id : g[u])
					if (eg[id][2] > 0)
						mn = min(mn, cost(id));
				p[u] -= (mn / eps + 1) * eps, q.push({ -p[u], u });
			}
		}
	}

};
int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	ll n, m;
	cin >> n >> m;
	network nt(n);
	for (ll i = 0; i < m; ++i) {
		ll u, v, f, c;
		cin >> u >> v >> f >> c;
		--u;
		--v;
		nt.add(u, v, f, c);
	}
	pll ans = nt.mincostmaxflow(0, n - 1);

	cout << "flow, cost = " << ans[0] << ' ' << ans[1] << endl;




	return 0;
}
