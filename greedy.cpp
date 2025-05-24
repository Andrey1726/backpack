#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Item {
    int weight;
    int value;
    double ratio;
};

bool compare(const Item &a, const Item &b) {
    return a.ratio > b.ratio;
}

double fractionalKnapsack(int W, vector<Item>& items) {
    sort(items.begin(), items.end(), compare);
    
    double totalValue = 0.0;
    int remainingWeight = W;
    
    for (const auto& item : items) {
        if (remainingWeight <= 0) break;
        
        if (item.weight <= remainingWeight) {
            totalValue += item.value;
            remainingWeight -= item.weight;
        } else {
            double fraction = (double)remainingWeight / item.weight;
            totalValue += item.value * fraction;
            remainingWeight = 0;
        }
    }
    
    return totalValue;
}

int main() {
    int W, n;
    cin >> n;
    cin >> W;
    
    vector<Item> items(n);
    for (int i = 0; i < n; ++i) {
        cin >> items[i].weight >> items[i].value;
        items[i].ratio = (double)items[i].value / items[i].weight;
    }
    
    clock_t start = clock();
    double maxValue = fractionalKnapsack(W, items);
    cout << maxValue << endl;
    clock_t end = clock();
    double time = (double) (end-start)/CLOCKS_PER_SEC;
    cout << time<<endl;
    return 0;
}