# Pthreads ile Bariyer Senkronizasyonu ve Performans Analizi

Bu proje, C++ ve POSIX Threads (pthreads) kütüphanesi kullanılarak çoklu iş parçacığı (multi-threading) ortamlarında **Bariyer (Barrier)** senkronizasyonunun nasıl yapıldığını gösterir. Projede **Busy-Wait (Aktif Bekleme)** ve **Condition Variable (Koşul Değişkeni)** yöntemleri uygulanmış ve performansları karşılaştırılmıştır.

## 🚀 Proje Özeti

Program, büyük bir global dizi üzerinde iki aşamalı bir hesaplama gerçekleştirmek için `N` adet thread oluşturur:

1.  **Faz 1 (Paralel Hesaplama):** Her thread, kendisine atanan aralıktaki sayıların karesini hesaplar.
2.  **Bariyer Noktası:** Tüm threadler Faz 1'i bitirene kadar bu noktada bekler.
3.  **Faz 2 (Birleştirme/Toplama):** Bariyer geçildikten sonra, her thread kendi aralığındaki sayıların toplamını hesaplayıp genel toplama ekler.

### Uygulanan Yöntemler

Bariyer mekanizması için iki farklı yaklaşım kodlanmıştır:

1.  **Busy-Wait + Mutex (`busy_wait.cpp`):**
    - Mutex ile korunan global bir sayaç kullanılır.
    - Threadler, sayaç hedefe ulaşana kadar `while` döngüsü içinde sürekli kontrol yapar.
    - **Gözlem:** İşlemci (CPU) sürekli meşgul edildiği için kaynak kullanımı yüksektir.

2.  **Condition Variable (`cond_var.cpp`):**
    - `pthread_cond_wait` ve `pthread_cond_broadcast` fonksiyonları kullanılır.
    - Threadler bariyer noktasına geldiğinde "uyku" (blocked) moduna geçer.
    - **Gözlem:** Bekleme süresince CPU kullanımı %0'dır, sistem kaynakları verimli kullanılır.

## 🛠️ Teknolojiler

* **Dil:** C++
* **Kütüphane:** pthread (POSIX Threads)
* **Platform:** Cross-platform (Windows üzerinde MinGW ile geliştirildi)

## ⚙️ Kurulum ve Çalıştırma

Kodları derlemek için pthreads destekleyen bir C++ derleyicisine (GCC/MinGW gibi) ihtiyacınız vardır.

Busy-Wait Çözümünü Derle ve Çalıştır
g++ busy_wait.cpp -o busy.exe -lpthread
./busy.exe

Condition Variable Çözümünü Derle ve Çalıştır
g++ cond_var.cpp -o cond.exe -lpthread
./cond.exe
