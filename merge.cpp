#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>

using namespace std;

// --- Simplified Merge Function ---
void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp;
    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) temp.push_back(arr[i++]);
        else temp.push_back(arr[j++]);
    }

    while (i <= mid) temp.push_back(arr[i++]);
    while (j <= right) temp.push_back(arr[j++]);

    for (int k = 0; k < temp.size(); k++) {
        arr[left + k] = temp[k];
    }
}

// --- 1. Sequential Merge Sort ---
void sequential_merge_sort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        sequential_merge_sort(arr, left, mid);
        sequential_merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// --- 2. Parallel Merge Sort ---
void parallel_merge_sort(vector<int>& arr, int left, int right) {
    if (right - left < 1000) {
        sequential_merge_sort(arr, left, right);
        return;
    }

    if (left < right) {
        int mid = left + (right - left) / 2;

        #pragma omp task shared(arr)
        parallel_merge_sort(arr, left, mid);

        #pragma omp task shared(arr)
        parallel_merge_sort(arr, mid + 1, right);

        #pragma omp taskwait

        merge(arr, left, mid, right);
    }
}

int main() {
    int N;
    
    cout << "Enter the number of elements to sort (Try 1000000+ for best results): ";
    cin >> N;

    if (N <= 0) {
        cout << "Please enter a valid positive number." << endl;
        return 1;
    }

    vector<int> arr_seq(N);
    vector<int> arr_par(N);

    srand(42); 
    cout << "Generating " << N << " random numbers...\n" << endl;
    for (int i = 0; i < N; i++) {
        int val = rand() % 100000;
        arr_seq[i] = val;
        arr_par[i] = val; 
    }

    double start_seq = omp_get_wtime();
    sequential_merge_sort(arr_seq, 0, N - 1);
    double end_seq = omp_get_wtime();
    double time_seq = end_seq - start_seq;
    cout << "Sequential Merge Sort Time: \t" << time_seq << " seconds\n";

    double start_par = omp_get_wtime();
    
    #pragma omp parallel
    {
        #pragma omp single
        parallel_merge_sort(arr_par, 0, N - 1);
    }
    
    double end_par = omp_get_wtime();
    double time_par = end_par - start_par;
    cout << "Parallel Merge Sort Time: \t" << time_par << " seconds\n";

    if (time_par > 0) {
        double speedup = time_seq / time_par;
        cout << "\nSpeedup: \t\t\t" << speedup << "x" << endl;
    }
    
    return 0;
}
