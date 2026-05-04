#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

const int MAXN = 100000;
vector<int> adj[MAXN + 5]; // adjacency list
bool visited[MAXN + 5];    // mark visited nodes

void dfs(int node) {
    // 1. Thread-safe check and update. 
    // We must lock this check so multiple threads don't visit the same node simultaneously.
    bool is_first_visit = false;
    #pragma omp critical
    {
        if (!visited[node]) {
            visited[node] = true;
            is_first_visit = true;
        }
    }

    // If another thread already claimed and visited this node, stop.
    if (!is_first_visit) return;

    // 2. Iterate through neighbors and spawn OpenMP tasks
    for (int i = 0; i < adj[node].size(); i++) {
        int next_node = adj[node][i];
        
        // Fast-path check: Only spawn a task if it looks unvisited.
        // The actual source of truth is the critical section above.
        if (!visited[next_node]) {
            // Spawn a new task for the recursive call
            #pragma omp task shared(adj, visited)
            dfs(next_node);
        }
    }
}

int main() {
    cout << "Please enter nodes and edges: ";
    int n, m; 
    cin >> n >> m;
    
    for (int i = 1; i <= m; i++) {
        int u, v; 
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    
    cout << "Enter start node: ";
    int start_node; 
    cin >> start_node;

    // 3. Open a single parallel region at the very beginning
    #pragma omp parallel
    {
        // 4. Use 'single' so only ONE thread kicks off the first recursive task
        #pragma omp single
        dfs(start_node);
    }

    // Print visited nodes
    cout << "Visited nodes: ";
    for (int i = 1; i <= n; i++) {
        if (visited[i]) {
            cout << i << " ";
        }
    }
    cout << endl;
    
    return 0;
}
