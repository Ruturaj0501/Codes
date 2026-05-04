#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

// Function to perform Parallel BFS
void bfs_parallel(int start_node, int n, const vector<vector<int>>& adj) {
    vector<bool> visited(n + 1, false);
    vector<int> current_frontier;

    // Initialize the search with the source node
    visited[start_node] = true;
    current_frontier.push_back(start_node);

    cout << "BFS Traversal Order: " << endl;

    while (!current_frontier.empty()) {
        // Print nodes at the current level
        for (int node : current_frontier) {
            cout << node << " ";
        }
        
        vector<int> next_frontier;

        // Start parallel region to process the current level's neighbors
        #pragma omp parallel
        {
            vector<int> local_next; // Private list for each thread

            #pragma omp for
            for (int i = 0; i < current_frontier.size(); i++) {
                int u = current_frontier[i];
                for (int v : adj[u]) {
                    bool first_visit = false;

                    // Critical section to ensure thread-safe marking of 'visited'
                    #pragma omp critical
                    {
                        if (!visited[v]) {
                            visited[v] = true;
                            first_visit = true;
                        }
                    }

                    if (first_visit) {
                        local_next.push_back(v);
                    }
                }
            }

            // Merge local discoveries into the global frontier for the next level
            #pragma omp critical
            {
                next_frontier.insert(next_frontier.end(), local_next.begin(), local_next.end());
            }
        }
        
        current_frontier = next_frontier; // Move to the next depth level
    }
    cout << endl;
}

int main() {
    int n, m, source;
    cout << "Enter number of vertices and edges: ";
    cin >> n >> m;

    vector<vector<int>> adj(n + 1);
    cout << "Enter edges (u v):" << endl;
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    cout << "Enter source node: ";
    cin >> source;

    double start_time = omp_get_wtime();
    bfs_parallel(source, n, adj);
    double end_time = omp_get_wtime();

    cout << "\nTime taken: " << end_time - start_time << " seconds" << endl;

    return 0;
}
