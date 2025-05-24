#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <numeric>
#include <climits>

using namespace std;

struct Item {
    int weight;
    int value;
};

class GeneticKnapsack {
private:
    vector<Item> items;
    int capacity;
    int populationSize;
    double mutationRate;
    int maxGenerations;
    int tournamentSize;
    int elitismCount;
    
    struct Chromosome {
        vector<bool> genes;
        double fitness;
        int totalWeight;
        
        Chromosome(int size) : genes(size, false), fitness(0), totalWeight(0) {}
    };
    
    vector<Chromosome> population;
    mt19937 rng;
    
    void initializePopulation() {
        uniform_int_distribution<int> randGene(0, 1);
        
        for (int i = 0; i < populationSize; ++i) {
            Chromosome chromosome(items.size());
            
            for (int j = 0; j < items.size(); ++j) {
                chromosome.genes[j] = randGene(rng);
            }
            
            repairChromosome(chromosome);
            calculateFitness(chromosome);
            population.push_back(chromosome);
        }
    }
    
    void repairChromosome(Chromosome& chromosome) {
        while (chromosome.totalWeight > capacity && chromosome.fitness > 0) {
            int worstIndex = -1;
            double worstRatio = INT_MAX;
            
            for (int i = 0; i < items.size(); ++i) {
                if (chromosome.genes[i]) {
                    double ratio = (double)items[i].value / items[i].weight;
                    if (ratio < worstRatio) {
                        worstRatio = ratio;
                        worstIndex = i;
                    }
                }
            }
            
            if (worstIndex != -1) {
                chromosome.genes[worstIndex] = false;
                chromosome.totalWeight -= items[worstIndex].weight;
                chromosome.fitness -= items[worstIndex].value;
            }
        }
    }
    
    void calculateFitness(Chromosome& chromosome) {
        chromosome.totalWeight = 0;
        chromosome.fitness = 0;
        
        for (int i = 0; i < items.size(); ++i) {
            if (chromosome.genes[i]) {
                chromosome.totalWeight += items[i].weight;
                chromosome.fitness += items[i].value;
            }
        }
        
        if (chromosome.totalWeight > capacity) {
            repairChromosome(chromosome);
        }
    }
    
    Chromosome tournamentSelection() {
        uniform_int_distribution<int> randPop(0, populationSize - 1);
        Chromosome best = population[randPop(rng)];
        
        for (int i = 1; i < tournamentSize; ++i) {
            Chromosome contender = population[randPop(rng)];
            if (contender.fitness > best.fitness) {
                best = contender;
            }
        }
        
        return best;
    }
    
    void crossoverAndMutation() {
        vector<Chromosome> newPopulation;
        
        sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.fitness > b.fitness;
        });
        
        for (int i = 0; i < elitismCount; ++i) {
            newPopulation.push_back(population[i]);
        }
        
        uniform_real_distribution<double> randProb(0.0, 1.0);
        uniform_int_distribution<int> randPoint(1, items.size() - 2);
        
        while (newPopulation.size() < populationSize) {
            Chromosome parent1 = tournamentSelection();
            Chromosome parent2 = tournamentSelection();
            
            Chromosome child1(items.size()), child2(items.size());
            
            int crossoverPoint = randPoint(rng);
            
            for (int i = 0; i < items.size(); ++i) {
                if (i < crossoverPoint) {
                    child1.genes[i] = parent1.genes[i];
                    child2.genes[i] = parent2.genes[i];
                } else {
                    child1.genes[i] = parent2.genes[i];
                    child2.genes[i] = parent1.genes[i];
                }
            }
            
            for (int i = 0; i < items.size(); ++i) {
                if (randProb(rng) < mutationRate) {
                    child1.genes[i] = !child1.genes[i];
                }
                if (randProb(rng) < mutationRate) {
                    child2.genes[i] = !child2.genes[i];
                }
            }
            
            repairChromosome(child1);
            calculateFitness(child1);
            repairChromosome(child2);
            calculateFitness(child2);
            
            newPopulation.push_back(child1);
            if (newPopulation.size() < populationSize) {
                newPopulation.push_back(child2);
            }
        }
        
        population = newPopulation;
    }
    
public:
    GeneticKnapsack(const vector<Item>& items, int capacity, 
                   int popSize = 200, double mutRate = 0.05, 
                   int maxGen = 500, int tourSize = 5, int elitism = 2)
        : items(items), capacity(capacity), populationSize(popSize), 
          mutationRate(mutRate), maxGenerations(maxGen), 
          tournamentSize(tourSize), elitismCount(elitism) {
        rng.seed(time(nullptr));
    }
    
    double solve() {
        initializePopulation();
        
        for (int generation = 0; generation < maxGenerations; ++generation) {
            crossoverAndMutation();
        }
        
        auto best = max_element(population.begin(), population.end(), 
                              [](const Chromosome& a, const Chromosome& b) {
                                  return a.fitness < b.fitness;
                              });
        
        return best->fitness;
    }
};

int main() {
    int W, n;
    cin >> n;
    cin >> W;
    
    vector<Item> items(n);
    for (int i = 0; i < n; ++i) {
        cin >> items[i].value >> items[i].weight;
    }

    clock_t start = clock();
    GeneticKnapsack solver(items, W);
    double maxValue = solver.solve();
    cout << maxValue << endl;
    clock_t end = clock();
    double time = (double) (end-start)/CLOCKS_PER_SEC;
    cout << time<<endl;
    return 0;
}