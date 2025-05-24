#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <chrono>

using namespace std;

struct Item {
    int weight;
    int value;
    double ratio; 
};

struct Node {
    int level;
    int profit;
    int weight;
    double bound;
};

bool compareItems(const Item &a, const Item &b) {
    return a.ratio > b.ratio;
}

double bound(Node u, int n, int W, const vector<Item> &items) {
    if (u.weight >= W)
        return 0;

    double profit_bound = u.profit;
    int j = u.level + 1;
    int total_weight = u.weight;

    while (j < n && total_weight + items[j].weight <= W) {
        total_weight += items[j].weight;
        profit_bound += items[j].value;
        j++;
    }

    if (j < n)
        profit_bound += (W - total_weight) * items[j].ratio;

    return profit_bound;
}

atomic<int> max_profit(0);
mutex mtx;
queue<Node> task_queue;
atomic<bool> done(false);

void knapsack_thread(int n, int W, const vector<Item> &items) {
    while (!done) {
        Node u;
        bool task_available = false;
        
        {
            lock_guard<mutex> lock(mtx);
            if (!task_queue.empty()) {
                u = task_queue.front();
                task_queue.pop();
                task_available = true;
            }
        }
        
        if (!task_available) {
            if (done) break;
            this_thread::yield();
            continue;
        }
        
        if (u.bound <= max_profit) {
            continue;
        }
        
        if (u.profit > max_profit) {
            int old_max = max_profit;
            while (u.profit > old_max && !max_profit.compare_exchange_weak(old_max, u.profit)) {
                old_max = max_profit;
            }
        }
        
        if (u.level == n - 1) {
            continue;
        }
        
        Node v;
        v.level = u.level + 1;
        v.weight = u.weight + items[v.level].weight;
        v.profit = u.profit + items[v.level].value;
        
        if (v.weight <= W && v.profit > max_profit) {
            max_profit = v.profit;
        }
        
        v.bound = bound(v, n, W, items);
        if (v.bound > max_profit) {
            lock_guard<mutex> lock(mtx);
            task_queue.push(v);
        }
        
        Node w;
        w.level = u.level + 1;
        w.weight = u.weight;
        w.profit = u.profit;
        w.bound = bound(w, n, W, items);
        
        if (w.bound > max_profit) {
            lock_guard<mutex> lock(mtx);
            task_queue.push(w);
        }
    }
}

int knapsack(int W, vector<Item> &items) {
    for (auto &item : items) {
        item.ratio = (double)item.value / item.weight;
    }
    sort(items.begin(), items.end(), compareItems);
    
    int n = items.size();
    
    Node u;
    u.level = -1;
    u.profit = 0;
    u.weight = 0;
    u.bound = bound(u, n, W, items);
    
    task_queue.push(u);
    
    unsigned num_threads = 8;
    vector<thread> threads;
    
    for (unsigned i = 0; i < num_threads; ++i) {
        threads.emplace_back(knapsack_thread, n, W, items);
    }
    
    while (true) {
        {
            lock_guard<mutex> lock(mtx);
            if (task_queue.empty()) {
                done = true;
                break;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    
    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    return max_profit;
}

int main() {
    int W; 
    int n; 
    cin >> n;
    cin >> W;
    
    vector<Item> items(n);
    for (int i = 0; i < n; i++) {
        cin >> items[i].value >> items[i].weight;
    }
    
    max_profit = 0;
    done = false;
    
    auto start = chrono::high_resolution_clock::now();
    int result = knapsack(W, items);
    auto end = chrono::high_resolution_clock::now();
    
    cout << result << endl;
    cout << chrono::duration<double>(end - start).count() << endl;
    
    return 0;
}