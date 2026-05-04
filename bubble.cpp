#include <iostream>
#include <vector>
#include <omp.h>
#include <algorithm> // for std::swap

using namespace std;

// 1. Standard Sequential Bubble Sort
void sequential_bubble_sort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break; 
    }
}

// 2. Parallel Bubble Sort (Odd-Even Transposition)
void parallel_odd_even_sort(vector<int>& arr) {
    int n = arr.size();
    
    #pragma omp parallel
    {
        for (int phase = 0; phase < n; phase++) {
            if (phase % 2 == 0) {
                // Even phase
                #pragma omp for
                for (int i = 0; i < n - 1; i += 2) {
                    if (arr[i] > arr[i + 1]) {
                        swap(arr[i], arr[i + 1]);
                    }
                }
            } else {
                // Odd phase
                #pragma omp for
                for (int i = 1; i < n - 1; i += 2) {
                    if (arr[i] > arr[i + 1]) {
                        swap(arr[i], arr[i + 1]);
                    }
                }
            }
        }
    }
}

int main() {
    int N;
    
    // 1. Get array size from user
    cout << "Enter the number of elements: ";
    cin >> N;

    if (N <= 0) {
        cout << "Please enter a valid positive number." << endl;
        return 1;
    }

    vector<int> arr_seq(N);
    vector<int> arr_par(N);

    // 2. Get array elements from user
    cout << "Enter " << N << " integers separated by spaces:\n";
    for (int i = 0; i < N; i++) {
        int val;
        cin >> val;
        arr_seq[i] = val;
        arr_par[i] = val; // Keep copies identical for a fair race
    }

    cout << "\nSorting..." << endl;

    // --- Measure Sequential Performance ---
    double start_seq = omp_get_wtime();
    sequential_bubble_sort(arr_seq);
    double end_seq = omp_get_wtime();
    double time_seq = end_seq - start_seq;
    
    // --- Measure Parallel Performance ---
    double start_par = omp_get_wtime();
    parallel_odd_even_sort(arr_par);
    double end_par = omp_get_wtime();
    double time_par = end_par - start_par;

    // --- Output Results ---
    cout << "\nSorted array: ";
    for (int num : arr_par) {
        cout << num << " ";
    }
    
    cout << "\n\nSequential Time: \t" << time_seq << " seconds\n";
    cout << "Parallel Time: \t\t" << time_par << " seconds\n";

    if (time_par > 0) {
        double speedup = time_seq / time_par;
        cout << "Speedup: \t\t" << speedup << "x" << endl;
    }
    
    return 0;
}
