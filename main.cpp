#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>


using namespace std;

// ======================================================================
// BLOK 1: Struct & Array (Tugas: Andika Rahman Saula - 25.11.6307)
// ======================================================================

struct Mahasiswa {
  string nim; // Diubah menjadi string untuk mendukung format bertitik
              // (e.g. 25.11.6307)
  string nama;
  float nilaiKuis[3]; // Implementasi Array statis untuk nilai kuis
  float nilaiAkhir;
  char grade;
  Mahasiswa *next; // Pointer untuk node Linked List
};

struct QueueNode {
  string nim; // Diubah menjadi string
  QueueNode *next;
};

// ======================================================================
// BLOK QUALITY CONTROL: Validasi Input Terpusat (Tugas: Andika - 25.11.6307)
// ======================================================================

// Validasi input angka bulat (Integer)
int inputInt(const string &prompt) {
  string input;
  int val;
  while (true) {
    cout << prompt;
    getline(cin, input);
    
    bool allSpace = true;
    for (char c : input) {
        if (!isspace((unsigned char)c)) { allSpace = false; break; }
    }
    if (allSpace) {
        cout << "Error: Input tidak boleh kosong! Silakan coba lagi.\n";
        continue;
    }
    
    try {
        size_t pos;
        val = stoi(input, &pos);
        bool valid = true;
        for (size_t i = pos; i < input.length(); i++) {
            if (!isspace((unsigned char)input[i])) { valid = false; break; }
        }
        if (valid) return val;
    } catch (...) {}
    
    cout << "Error: Input harus berupa angka bulat (integer)! Silakan coba lagi.\n";
  }
}

// Validasi input angka desimal (Float) dengan batas rentang
float inputFloat(const string &prompt, float minVal, float maxVal) {
  string input;
  float val;
  while (true) {
    cout << prompt;
    getline(cin, input);
    
    bool allSpace = true;
    for (char c : input) {
        if (!isspace((unsigned char)c)) { allSpace = false; break; }
    }
    if (allSpace) {
        cout << "Error: Input tidak boleh kosong! Silakan coba lagi.\n";
        continue;
    }
    
    try {
        size_t pos;
        val = stof(input, &pos);
        bool valid = true;
        for (size_t i = pos; i < input.length(); i++) {
            if (!isspace((unsigned char)input[i])) { valid = false; break; }
        }
        if (valid && val >= minVal && val <= maxVal) return val;
        if (valid) {
            cout << "Error: Input harus angka desimal antara " << minVal << " dan " << maxVal << "! Silakan coba lagi.\n";
            continue;
        }
    } catch (...) {}
    
    cout << "Error: Input harus angka desimal antara " << minVal << " dan " << maxVal << "! Silakan coba lagi.\n";
  }
}

// Validasi input NIM (String bertitik, hanya menerima angka dan titik)
string inputNIM(const string &prompt) {
  string val;
  while (true) {
    cout << prompt;
    getline(cin, val);
    
    bool allSpace = true;
    for (char c : val) {
        if (!isspace((unsigned char)c)) { allSpace = false; break; }
    }
    if (allSpace) {
        cout << "Error: Input tidak boleh kosong! Silakan coba lagi.\n";
        continue;
    }

    bool valid = true;
    for (size_t i = 0; i < val.length(); i++) {
      if (!isdigit((unsigned char)val[i]) && val[i] != '.') {
        valid = false;
        break;
      }
    }

    if (valid) {
      return val;
    }
    cout << "Error: NIM hanya boleh berisi angka dan tanda titik! Silakan coba lagi.\n";
  }
}

// Validasi input kalimat/kata (String) terhindar dari sisa buffer \n
string inputString(const string &prompt) {
  string val;
  while (true) {
    cout << prompt;
    getline(cin, val);
    
    bool allSpace = true;
    for (char c : val) {
        if (!isspace((unsigned char)c)) { allSpace = false; break; }
    }
    if (allSpace) {
        cout << "Error: Input tidak boleh kosong! Silakan coba lagi.\n";
        continue;
    }
    return val;
  }
}

// ======================================================================
// KELAS UTAMA: ENKAPSULASI, PERSISTENSI, & MANAJEMEN MEMORI (OOP)
// ======================================================================

class ManajemenMahasiswa {
private:
  // ======================================================================
  // BLOK LINKED LIST CORE: Node Pointers (Tugas: Haikal Abrar - 25.11.6328)
  // ======================================================================
  Mahasiswa *head;
  Mahasiswa *tail;

  // ======================================================================
  // BLOK STACK & QUEUE MEMORY: Node Pointers (Tugas: Dwi Risqi - 25.11.6553)
  // ======================================================================
  Mahasiswa *undoStack;
  QueueNode *qFront;
  QueueNode *qRear;

  // Helper: Hitung nilai akhir dan grade (Tugas: Andika Rahman Saula
  // - 25.11.6307)
  void hitungNilai(Mahasiswa *m) {
    m->nilaiAkhir = (m->nilaiKuis[0] + m->nilaiKuis[1] + m->nilaiKuis[2]) / 3.0;
    if (m->nilaiAkhir >= 80.0) {
      m->grade = 'A';
    } else if (m->nilaiAkhir >= 70.0) {
      m->grade = 'B';
    } else if (m->nilaiAkhir >= 60.0) {
      m->grade = 'C';
    } else if (m->nilaiAkhir >= 50.0) {
      m->grade = 'D';
    } else {
      m->grade = 'E';
    }
  }

  // Helper: Memotong nama agar rapi pada tampilan tabel (Tugas: Andika Rahman
  // Saula - 25.11.6307) Digunakan bersama oleh tampilData() dan rankingNilai()
  // agar tidak ada duplikasi logika
  static string formatNama(const string &nama) {
    if (nama.length() > 17) {
      return nama.substr(0, 14) + "...";
    }
    return nama;
  }

  // Helper: Mencetak satu baris data mahasiswa pada tabel (Tugas: Razaan
  // Azzaryan Bilhaq - 25.11.6309) rank < 0 artinya tabel tanpa kolom Rank
  // (dipakai oleh tampilData)
  static void cetakBarisMahasiswa(const Mahasiswa *m, int rank = -1) {
    if (rank >= 0) {
      cout << left << setw(8) << rank;
    }
    cout << left << setw(12) << m->nim << setw(20) << formatNama(m->nama)
         << right << setw(8) << fixed << setprecision(1) << m->nilaiKuis[0]
         << setw(8) << m->nilaiKuis[1] << setw(8) << m->nilaiKuis[2] << setw(10)
         << m->nilaiAkhir << setw(6) << m->grade << "\n";
  }

  // Helper: Mencari node mahasiswa berdasarkan NIM (Tugas: Haikal Abrar
  // - 25.11.6328)
  Mahasiswa *cariNodeMhs(const string &nim) {
    Mahasiswa *current = head;
    while (current != NULL) {
      if (current->nim == nim) {
        return current;
      }
      current = current->next;
    }
    return NULL;
  }

  // Helper: Membuat node mahasiswa baru (Tugas: Haikal Abrar - 25.11.6328)
  Mahasiswa *buatNodeBaru(const string &nim, const string &nama,
                          const float kuis[3]) {
    Mahasiswa *baru = new Mahasiswa();
    baru->nim = nim;
    baru->nama = nama;
    baru->nilaiKuis[0] = kuis[0];
    baru->nilaiKuis[1] = kuis[1];
    baru->nilaiKuis[2] = kuis[2];
    hitungNilai(baru);
    baru->next = NULL;
    return baru;
  }

  // Helper: Menyisipkan node di akhir list utama (Tugas: Haikal Abrar
  // - 25.11.6328)
  void insertLastList(Mahasiswa *m) {
    if (head == NULL) {
      head = m;
      tail = m;
    } else {
      tail->next = m;
      tail = m;
    }
  }

  // ======================================================================
  // BLOK STACK: Operasi Stack untuk Undo Hapus (Tugas: Dwi Risqi - 25.11.6553)
  // ======================================================================
  void pushUndo(Mahasiswa *m) {
    m->next = undoStack;
    undoStack = m;
  }

  Mahasiswa *popUndo() {
    if (undoStack == NULL) {
      return NULL;
    }
    Mahasiswa *temp = undoStack;
    undoStack = undoStack->next;
    temp->next = NULL;
    return temp;
  }

  // ======================================================================
  // BLOK QUEUE: Operasi Queue untuk Antrean Revisi (Tugas: Dwi Risqi
  // - 25.11.6553)
  // ======================================================================
  void enqueueRevisi(const string &nim) {
    QueueNode *baru = new QueueNode();
    baru->nim = nim;
    baru->next = NULL;
    if (qRear == NULL) {
      qFront = baru;
      qRear = baru;
    } else {
      qRear->next = baru;
      qRear = baru;
    }
  }

  bool dequeueRevisi(string &nim) {
    if (qFront == NULL) {
      return false;
    }
    QueueNode *temp = qFront;
    nim = temp->nim;
    qFront = qFront->next;
    if (qFront == NULL) {
      qRear = NULL;
    }
    delete temp;
    return true;
  }

  bool isMhsInQueue(const string &nim) {
    QueueNode *current = qFront;
    while (current != NULL) {
      if (current->nim == nim) {
        return true;
      }
      current = current->next;
    }
    return false;
  }

  // ======================================================================
  // BLOK INTEGRASI SISTEM: Penyimpanan File Aman (Tugas: Haikal Abrar
  // - 25.11.6328)
  // ======================================================================
  void simpanKeFile() {
    ofstream file("mahasiswa.tmp");
    if (!file.is_open())
      return;
    Mahasiswa *current = head;
    while (current != NULL) {
      file << current->nim << ";" << current->nama << ";"
           << current->nilaiKuis[0] << ";" << current->nilaiKuis[1] << ";"
           << current->nilaiKuis[2] << "\n";
      current = current->next;
    }
    file.close();
    remove("mahasiswa.txt");
    rename("mahasiswa.tmp", "mahasiswa.txt");
  }

  void muatDariFile() {
    ifstream file("mahasiswa.txt");
    if (!file.is_open())
      return;
    string nimStr, nama, temp;
    float kuis[3];
    int count = 0;
    while (getline(file, nimStr, ';')) {
      if (nimStr.empty() || nimStr == "\n" || nimStr == "\r")
        continue;
      // Langsung menggunakan nimStr bertipe string tanpa atoi
      getline(file, nama, ';');
      getline(file, temp, ';');
      kuis[0] = atof(temp.c_str());
      getline(file, temp, ';');
      kuis[1] = atof(temp.c_str());
      getline(file, temp, '\n');
      kuis[2] = atof(temp.c_str());
      insertLastList(buatNodeBaru(nimStr, nama, kuis));
      count++;
    }
    file.close();
    if (count > 0) {
      cout << "[Sistem] Berhasil memuat " << count << " data mahasiswa.\n";
    }
  }

  // Pembersihan memori dinamis terpusat di Destructor (Tugas: Haikal & Andika)
  void bersihkanMemori() {
    Mahasiswa *currentMhs = head;
    while (currentMhs != NULL) {
      Mahasiswa *temp = currentMhs;
      currentMhs = currentMhs->next;
      delete temp;
    }
    head = NULL;
    tail = NULL;

    Mahasiswa *currentUndo = undoStack;
    while (currentUndo != NULL) {
      Mahasiswa *temp = currentUndo;
      currentUndo = currentUndo->next;
      delete temp;
    }
    undoStack = NULL;

    QueueNode *currentQ = qFront;
    while (currentQ != NULL) {
      QueueNode *temp = currentQ;
      currentQ = currentQ->next;
      delete temp;
    }
    qFront = NULL;
    qRear = NULL;
  }

public:
  // Integrasi Sistem (Tugas: Haikal Abrar - 25.11.6328)
  ManajemenMahasiswa() {
    head = NULL;
    tail = NULL;
    undoStack = NULL;
    qFront = NULL;
    qRear = NULL;
    muatDariFile();
  }

  // Destructor (Tugas: Haikal Abrar - 25.11.6328)
  ~ManajemenMahasiswa() { bersihkanMemori(); }

  // ======================================================================
  // BLOK CRUD: Fungsi Operasi Data (Tugas: Dwi Risqi Ramadhan - 25.11.6553)
  // ======================================================================

  void tambahData() {
    cout << "\n-----------------------------------------\n"
         << "          TAMBAH DATA MAHASISWA          \n"
         << "-----------------------------------------\n";
    string nim = inputNIM("Masukkan NIM  : ");
    if (cariNodeMhs(nim) != NULL) {
      cout << "Error: Mahasiswa dengan NIM " << nim << " sudah terdaftar!\n";
      return;
    }
    string nama = inputString("Masukkan Nama : ");
    float kuis[3];
    kuis[0] = inputFloat("Masukkan Nilai Kuis 1 (0-100): ", 0.0, 100.0);
    kuis[1] = inputFloat("Masukkan Nilai Kuis 2 (0-100): ", 0.0, 100.0);
    kuis[2] = inputFloat("Masukkan Nilai Kuis 3 (0-100): ", 0.0, 100.0);

    insertLastList(buatNodeBaru(nim, nama, kuis));
    simpanKeFile();
    cout << "\nData mahasiswa \"" << nama << "\" berhasil ditambahkan!\n";
  }

  void tampilData() {
    if (head == NULL) {
      cout << "Belum ada data mahasiswa.\n";
      return;
    }
    cout << "\n================================================================"
            "====\n"
         << "                      DAFTAR NILAI MAHASISWA                      "
            "  \n"
         << "=================================================================="
            "==\n"
         << left << setw(12) << "NIM" << setw(20) << "Nama" << right << setw(8)
         << "K1" << setw(8) << "K2" << setw(8) << "K3" << setw(10) << "Akhir"
         << setw(6) << "Grade" << "\n"
         << "------------------------------------------------------------------"
            "--\n";
    Mahasiswa *current = head;
    while (current != NULL) {
      cetakBarisMahasiswa(current);
      current = current->next;
    }
    cout << "=================================================================="
            "==\n";
  }

  void editData() {
    cout << "\n-----------------------------------------\n"
         << "           EDIT DATA MAHASISWA           \n"
         << "-----------------------------------------\n";
    string nim = inputNIM("Masukkan NIM: ");
    Mahasiswa *m = cariNodeMhs(nim);
    if (m == NULL) {
      cout << "Mahasiswa dengan NIM " << nim << " tidak ditemukan.\n";
      return;
    }
    cout << "Nama Saat Ini: " << m->nama << "\n\n";
    m->nama = inputString("Masukkan Nama Baru: ");
    m->nilaiKuis[0] =
        inputFloat("Masukkan Nilai Kuis 1 Baru (0-100): ", 0.0, 100.0);
    m->nilaiKuis[1] =
        inputFloat("Masukkan Nilai Kuis 2 Baru (0-100): ", 0.0, 100.0);
    m->nilaiKuis[2] =
        inputFloat("Masukkan Nilai Kuis 3 Baru (0-100): ", 0.0, 100.0);
    hitungNilai(m);
    simpanKeFile();
    cout << "\nData berhasil diperbarui!\n";
  }

  void hapusData() {
    if (head == NULL) {
      cout << "Data kosong.\n";
      return;
    }
    cout << "\n-----------------------------------------\n"
         << "          HAPUS DATA MAHASISWA           \n"
         << "-----------------------------------------\n";
    string nim = inputNIM("Masukkan NIM mahasiswa yang akan dihapus: ");
    Mahasiswa *current = head;
    Mahasiswa *prev = NULL;
    while (current != NULL && current->nim != nim) {
      prev = current;
      current = current->next;
    }
    if (current == NULL) {
      cout << "Mahasiswa tidak ditemukan.\n";
      return;
    }

    if (prev == NULL) {
      head = head->next;
    } else {
      prev->next = current->next;
    }
    if (current == tail) {
      tail = prev;
    }

    pushUndo(current);
    simpanKeFile();
    cout << "\nData \"" << current->nama
         << "\" berhasil dihapus! (Bisa dibatalkan dengan Undo Hapus)\n";
  }

  // Stack: Undo Hapus (Tugas: Dwi Risqi Ramadhan - 25.11.6553)
  void undoHapus() {
    cout << "\n-----------------------------------------\n"
         << "               UNDO HAPUS                \n"
         << "-----------------------------------------\n";
    Mahasiswa *m = popUndo();
    if (m == NULL) {
      cout << "Tidak ada data untuk di-undo (Stack kosong).\n";
      return;
    }
    if (cariNodeMhs(m->nim) != NULL) {
      cout << "Gagal memulihkan. NIM " << m->nim
           << " sudah terdaftar kembali.\n";
      delete m;
      return;
    }
    insertLastList(m);
    simpanKeFile();
    cout << "Undo Sukses! Data \"" << m->nama << "\" dikembalikan.\n";
  }

  // Queue: Antrean Revisi Nilai (Tugas: Dwi Risqi Ramadhan - 25.11.6553)
  void antreanRevisi() {
    int sub;
    do {
      cout << "\n-----------------------------------------\n"
           << "       ANTREAN REVISI NILAI (QUEUE)      \n"
           << "-----------------------------------------\n"
           << "1. Tambah Antrean Revisi (Enqueue)\n"
           << "2. Proses Antrean Revisi (Dequeue)\n"
           << "3. Tampilkan Antrean Revisi\n"
           << "0. Kembali ke Menu Utama\n"
           << "-----------------------------------------\n";
      sub = inputInt("Pilih submenu: ");

      if (sub == 1) {
        string nim =
            inputNIM("Masukkan NIM mahasiswa yang mengajukan revisi: ");
        if (cariNodeMhs(nim) == NULL)
          cout << "Mahasiswa tidak ditemukan.\n";
        else if (isMhsInQueue(nim))
          cout << "Mahasiswa sudah berada di antrean.\n";
        else {
          enqueueRevisi(nim);
          cout << "Berhasil ditambahkan ke antrean.\n";
        }
      } else if (sub == 2) {
        string nim;
        if (dequeueRevisi(nim)) {
          Mahasiswa *m = cariNodeMhs(nim);
          if (m != NULL) {
            cout << "\nMemproses revisi: " << m->nama << "\n";
            m->nilaiKuis[0] =
                inputFloat("Masukkan Nilai Kuis 1 Baru (0-100): ", 0.0, 100.0);
            m->nilaiKuis[1] =
                inputFloat("Masukkan Nilai Kuis 2 Baru (0-100): ", 0.0, 100.0);
            m->nilaiKuis[2] =
                inputFloat("Masukkan Nilai Kuis 3 Baru (0-100): ", 0.0, 100.0);
            hitungNilai(m);
            simpanKeFile();
            cout << "Revisi selesai diproses.\n";
          } else
            cout << "Mahasiswa dalam antrean sudah dihapus.\n";
        } else
          cout << "Antrean revisi kosong.\n";
      } else if (sub == 3) {
        if (qFront == NULL)
          cout << "Antrean kosong.\n";
        else {
          int pos = 1;
          QueueNode *current = qFront;
          while (current != NULL) {
            Mahasiswa *m = cariNodeMhs(current->nim);
            cout << pos << ". NIM: " << current->nim
                 << " | Nama: " << (m ? m->nama : "[Data Terhapus]") << "\n";
            current = current->next;
            pos++;
          }
        }
      }
    } while (sub != 0);
  }

  // ======================================================================
  // BLOK SEARCHING: Pencarian Data (Tugas: Razaan Azzaryan Bilhaq - 25.11.6309)
  // ======================================================================
  void cariNIM() {
    cout << "\n-----------------------------------------\n"
         << "         CARI MAHASISWA (SEARCH)         \n"
         << "-----------------------------------------\n";
    string nim = inputNIM("Masukkan NIM mahasiswa yang dicari: ");
    Mahasiswa *m = cariNodeMhs(nim);
    if (m != NULL) {
      cout << "\nData Ditemukan!\n"
           << "NIM          : " << m->nim << "\n"
           << "Nama         : " << m->nama << "\n"
           << "Nilai Kuis   : " << m->nilaiKuis[0] << ", " << m->nilaiKuis[1]
           << ", " << m->nilaiKuis[2] << "\n"
           << "Nilai Akhir  : " << fixed << setprecision(2) << m->nilaiAkhir
           << "\n"
           << "Grade        : " << m->grade << "\n";
    } else
      cout << "Mahasiswa tidak ditemukan.\n";
  }

  // ======================================================================
  // BLOK SORTING: Pengurutan Data Ranking (Tugas: Razaan Azzaryan Bilhaq
  // - 25.11.6309)
  // ======================================================================

  // Bubble Sort manual menggunakan Array pointer ke node Linked List asli
  // (Efisiensi: tidak lagi menduplikasi seluruh node/string ke Linked List
  // sementara. Data asli pada Linked List tidak diubah urutannya karena
  // hanya pointer di dalam array yang ditukar, bukan isi node.)
  void rankingNilai() {
    if (head == NULL) {
      cout << "Data kosong.\n";
      return;
    }

    // 1. Hitung jumlah node lalu siapkan Array pointer sebesar jumlah data
    int jumlah = 0;
    for (Mahasiswa *c = head; c != NULL; c = c->next) {
      jumlah++;
    }
    Mahasiswa **urutan = new Mahasiswa *[jumlah];
    int idx = 0;
    for (Mahasiswa *c = head; c != NULL; c = c->next) {
      urutan[idx++] = c;
    }

    // 2. Bubble Sort manual pada Array pointer (hanya menukar alamat pointer)
    bool swapped;
    do {
      swapped = false;
      for (int i = 0; i < jumlah - 1; i++) {
        if (urutan[i]->nilaiAkhir < urutan[i + 1]->nilaiAkhir) {
          Mahasiswa *tempPtr = urutan[i];
          urutan[i] = urutan[i + 1];
          urutan[i + 1] = tempPtr;
          swapped = true;
        }
      }
    } while (swapped);

    // 3. Menampilkan hasil ranking berdasarkan urutan Array pointer
    cout << "\n================================================================"
            "====\n"
         << "                   RANKING NILAI MAHASISWA (BUBBLE SORT)          "
            "  \n"
         << "=================================================================="
            "==\n"
         << left << setw(8) << "Rank" << setw(12) << "NIM" << setw(20) << "Nama"
         << right << setw(8) << "K1" << setw(8) << "K2" << setw(8) << "K3"
         << setw(10) << "Akhir" << setw(6) << "Grade" << "\n"
         << "------------------------------------------------------------------"
            "--\n";

    for (int i = 0; i < jumlah; i++) {
      cetakBarisMahasiswa(urutan[i], i + 1);
    }
    cout << "=================================================================="
            "==\n";

    // 4. Membersihkan memori Array pointer (node asli TIDAK dihapus)
    delete[] urutan;
  }
};

// ======================================================================
// BLOK INTEGRASI SISTEM: Menu Controller (Tugas: Haikal & Andika)
// ======================================================================

int main() {
  ManajemenMahasiswa app;
  int pilihan;
  do {
    cout << "\n=========================================\n"
         << "   SISTEM MANAJEMEN NILAI MAHASISWA\n"
         << "=========================================\n"
         << "1. Tambah Data Mhs      5. Undo Hapus\n"
         << "2. Tampil Data Mhs      6. Antrean Revisi (Queue)\n"
         << "3. Edit Data Mhs        7. Cari Berdasarkan NIM\n"
         << "4. Hapus Data Mhs       8. Ranking Nilai Mhs\n"
         << "0. Keluar\n"
         << "=========================================\n";
    pilihan = inputInt("Pilih menu: ");
    cout << endl;

    switch (pilihan) {
    case 1:
      app.tambahData();
      break;
    case 2:
      app.tampilData();
      break;
    case 3:
      app.editData();
      break;
    case 4:
      app.hapusData();
      break;
    case 5:
      app.undoHapus();
      break;
    case 6:
      app.antreanRevisi();
      break;
    case 7:
      app.cariNIM();
      break;
    case 8:
      app.rankingNilai();
      break;
    case 0:
      cout << "Terima kasih telah menggunakan aplikasi ini.\n";
      break;
    default:
      cout << "Pilihan tidak tersedia. Silakan coba lagi.\n";
      break;
    }
  } while (pilihan != 0);

  return 0;
}
