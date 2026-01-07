#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h> // Sleep simülasyonu için (gerekirse)

using namespace std;

// --- GLOBAL DEĞİŞKENLER ---
const int N = 8;             // Thread sayısı
const int ARRAY_SIZE = 16000000; // Büyük bir dizi boyutu
int* A;                      // Global Dizi
long long global_sum = 0;    // Toplamın tutulacağı yer

// --- BARİYER İÇİN DEĞİŞKENLER ---
int arrived_count = 0;       // Bariyerdeki thread sayacı
pthread_mutex_t mutex_count; // Sayacı korumak için mutex
pthread_mutex_t mutex_sum;   // Toplamı korumak için mutex

// Thread verisi için struct
struct ThreadData {
    int id;
    int start_idx;
    int end_idx;
};

// BUSY-WAIT BARRIER FONKSİYONU
void barrier_busy_wait() {
    // 1. Sayacı güvenli bir şekilde artır
    pthread_mutex_lock(&mutex_count);
    arrived_count++;
    pthread_mutex_unlock(&mutex_count);

    // 2. Aktif Bekleme (Busy Wait)
    // Tüm threadler gelene kadar döngüde bekle.
    // Not: Gerçek hayatta bu değişken 'volatile' olmalı ki register'da cachelenmesin.
    while (arrived_count < N) {
        // CPU boş durmaz, sürekli kontrol eder.
    }
}

void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    // --- FAZ 1: Kare Hesaplama ---
    for (int i = data->start_idx; i < data->end_idx; ++i) {
        A[i] = A[i] * A[i];
    }

    // Bariyer Noktası
    barrier_busy_wait();

    // --- FAZ 2: Toplam Hesaplama ---
    long long local_sum = 0;
    for (int i = data->start_idx; i < data->end_idx; ++i) {
        local_sum += A[i];
    }

    // Global toplama ekle (Mutex ile korunmalı)
    pthread_mutex_lock(&mutex_sum);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex_sum);

    pthread_exit(NULL);
}

int main() {
    // Bellek ayırma ve başlatma
    A = new int[ARRAY_SIZE];
    for(int i=0; i<ARRAY_SIZE; i++) A[i] = 2; // Test için hepsi 2 olsun (karesi 4)

    pthread_t threads[N];
    ThreadData thread_data[N];

    // Mutexleri başlat
    pthread_mutex_init(&mutex_count, NULL);
    pthread_mutex_init(&mutex_sum, NULL);

    cout << "--- Busy-Wait Cozumu Baslatiliyor ---" << endl;

    int chunk = ARRAY_SIZE / N;
    for (int i = 0; i < N; ++i) {
        thread_data[i].id = i;
        thread_data[i].start_idx = i * chunk;
        thread_data[i].end_idx = (i == N - 1) ? ARRAY_SIZE : (i + 1) * chunk;
        
        pthread_create(&threads[i], NULL, thread_func, (void*)&thread_data[i]);
    }

    // Threadleri bekle
    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    cout << "Sonuc: " << global_sum << endl;
    cout << "Beklenen: " << (long long)ARRAY_SIZE * 4 << endl;

    // Temizlik
    pthread_mutex_destroy(&mutex_count);
    pthread_mutex_destroy(&mutex_sum);
    delete[] A;

    return 0;
}