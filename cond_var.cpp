#include <iostream>
#include <vector>
#include <pthread.h>

using namespace std;

// --- GLOBAL DEĞİŞKENLER ---
const int N = 8;             
const int ARRAY_SIZE = 16000000;
int* A;                      
long long global_sum = 0;    

// --- BARİYER İÇİN DEĞİŞKENLER ---
int arrived_count = 0;       
pthread_mutex_t mutex_barrier; // Bariyer kilidi
pthread_cond_t cond_barrier;   // Condition Variable
pthread_mutex_t mutex_sum;     

struct ThreadData {
    int id;
    int start_idx;
    int end_idx;
};

// CONDITION VARIABLE BARRIER FONKSİYONU
void barrier_cond_var() {
    pthread_mutex_lock(&mutex_barrier);
    arrived_count++;

    if (arrived_count < N) {
        // Son thread değilse uyu ve sinyal bekle
        pthread_cond_wait(&cond_barrier, &mutex_barrier);
    } else {
        // Son thread geldi, herkesi uyandır!
        pthread_cond_broadcast(&cond_barrier);
    }

    pthread_mutex_unlock(&mutex_barrier);
}

void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    // --- FAZ 1: Kare Hesaplama ---
    for (int i = data->start_idx; i < data->end_idx; ++i) {
        A[i] = A[i] * A[i];
    }

    // Bariyer Noktası
    barrier_cond_var();

    // --- FAZ 2: Toplam Hesaplama ---
    long long local_sum = 0;
    for (int i = data->start_idx; i < data->end_idx; ++i) {
        local_sum += A[i];
    }

    pthread_mutex_lock(&mutex_sum);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex_sum);

    pthread_exit(NULL);
    return NULL; // Derleyici uyarısını susturmak için
}

int main() {
    A = new int[ARRAY_SIZE];
    for(int i=0; i<ARRAY_SIZE; i++) A[i] = 2; 

    pthread_t threads[N];
    ThreadData thread_data[N];

    pthread_mutex_init(&mutex_barrier, NULL);
    pthread_cond_init(&cond_barrier, NULL);
    pthread_mutex_init(&mutex_sum, NULL);

    cout << "--- Condition Variable Cozumu Baslatiliyor ---" << endl;

    int chunk = ARRAY_SIZE / N;
    for (int i = 0; i < N; ++i) {
        thread_data[i].id = i;
        thread_data[i].start_idx = i * chunk;
        thread_data[i].end_idx = (i == N - 1) ? ARRAY_SIZE : (i + 1) * chunk;
        
        pthread_create(&threads[i], NULL, thread_func, (void*)&thread_data[i]);
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    cout << "Sonuc: " << global_sum << endl;
    cout << "Beklenen: " << (long long)ARRAY_SIZE * 4 << endl;

    pthread_mutex_destroy(&mutex_barrier);
    pthread_cond_destroy(&cond_barrier);
    pthread_mutex_destroy(&mutex_sum);
    delete[] A;

    return 0;
}