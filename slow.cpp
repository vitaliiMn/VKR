#include <bits/stdc++.h>
using namespace std;

#define all(x) x.begin(), x.end()
using ll = long long;
using pll = array<ll, 2>;


const ll inf = 1e18;
struct MCMF {
    struct edge {
        ll u, v, f, c, cost;
    };
    ll n;
    vector<edge> E;
    vector<vector<ll>> g;
    vector<ll> pt, prew, d;

    MCMF(ll _n) : n(_n), g(_n), pt(_n), prew(_n), d(_n) {}
    void add(ll u, ll v, ll c, ll cost) {
        g[u].push_back(E.size());
        E.push_back({ u, v, 0, c, cost });
        g[v].push_back(E.size());
        E.push_back({ v, u, 0, 0, -cost });
    }
    ll cost(ll id) { return E[id].c - E[id].f; }
    ll weight(ll id) { return E[id].cost + pt[E[id].u] - pt[E[id].v]; }

    void init(ll start) {
        fill(all(pt), inf);
        pt[start] = 0;
        for (ll _ = 0; _ < n; ++_)
            for (auto& [u, v, f, c, cost] : E)
                if (c - f > 0 && pt[u] < inf && pt[v] > pt[u] + cost)
                    pt[v] = pt[u] + cost;
    }
    void Deikstra(ll start) {
        fill(all(prew), -1);
        fill(all(d), inf);
        d[start] = 0;
        set<pll> st;
        st.insert({ d[start], start });
        while (!st.empty()) {
            ll u = (*st.begin())[1];
            st.erase(st.begin());
            for (ll id : g[u]) {
                if (cost(id) <= 0) continue;
                ll v = E[id].v;
                if (d[v] > d[u] + weight(id)) {
                    st.erase({ d[v], v });
                    d[v] = d[u] + weight(id);
                    prew[v] = id;
                    st.insert({ d[v], v });
                }
            }
        }
    }
    pll mincostflow(ll start, ll finish) {
        init(start);
        ll flow = 0, ans = 0;
        while (1) {
            Deikstra(start);
            if (prew[finish] == -1)break;
            ll f = inf;
            ll id = 0;
            for (ll cur = finish; cur != start; cur = E[id].u) {
                id = prew[cur];
                f = min(f, cost(id));
            }
            for (ll cur = finish; cur != start; cur = E[id].u) {
                id = prew[cur];
                ans += f * E[id].cost;
                E[id].f += f;
                E[id ^ 1].f -= f;
            }
            flow += f;
            Deikstra(start);
            for (ll i = 0; i < n; ++i)
                pt[i] = d[i] - pt[start];
        }
        return { flow, ans };
    }
};

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    ll n, m;
    cin >> n >> m;
    MCMF nt(n);
    for (ll i = 0; i < m; ++i) {
        ll u, v, f, c;
        cin >> u >> v >> f >> c;
        --u;
        --v;
        nt.add(u, v, f, c);
    }
    pll ans = nt.mincostflow(0, n - 1);

    cout << "flow, cost = " << ans[0] << ' ' << ans[1] << '\n';




    return 0;
}
