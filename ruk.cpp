#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Item {
    int value, weight;
};

struct Node {
    int level, profit, weight, bound;
};

bool pro(Item a, Item b) {
    return a.value/a.weight > b.value/b.weight;
}

int bound(Node u, int N, int W, vector<Item> &items) {
    if (u.weight >= W) return 0;

    int profit_bound = u.profit;
    int j = u.level + 1;
    int total_weight = u.weight;

    while (j < N && total_weight + items[j].weight <= W) {
        total_weight += items[j].weight;
        profit_bound += items[j].value;
        j++;
    }
    return profit_bound;
}

int BandB(int W,int N, vector<Item> &items) {
    queue<Node> Q; 
    Node u, v; 

    u.level = -1;
    u.profit = 0;
    u.weight = 0;
    u.bound = bound(u, N, W, items);

    Q.push(u);
    int max = 0;

    while (!Q.empty()) {
        u = Q.front();
        Q.pop();

        if (u.level != N - 1) {
            v.level = u.level + 1;
            v.weight = u.weight + items[v.level].weight;
            v.profit = u.profit + items[v.level].value;
            if (v.weight <= N && v.profit > max)
                max = v.profit;
            v.bound = bound(v, N, W, items);
            if (v.bound > max)
                Q.push(v);

            v.weight = u.weight;
            v.profit = u.profit;
            v.bound = bound(v, N, W, items);
            if (v.bound > max)
                Q.push(v);
        }
    }
    return max;
}

int main() {
    int W,N;
    cin >> W;
    cin >> N;
    vector<Item> items(N);
    for (Item n : items){
        cin >> n.value;
        cin >> n.weight;
    }
    sort(items.begin(), items.end(), pro);
    cout << BandB(W,N, items) << endl;
    return 0;
}
