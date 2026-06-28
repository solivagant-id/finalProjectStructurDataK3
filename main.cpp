#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cctype>

using namespace std;

// ======================================================================
// BLOK 1: Struct & Array (Tugas: Andika Rahman Saula - 25.11.6307)
// ======================================================================

struct Mahasiswa {
    string nim; // Diubah menjadi string untuk mendukung format bertitik (e.g. 25.11.6307)
    string nama;
    float nilaiKuis[3]; // Implementasi Array statis untuk nilai kuis
    float nilaiAkhir;
    char grade;
    Mahasiswa* next; // Pointer untuk node Linked List
};

struct QueueNode {
    string nim; // Diubah menjadi string
    QueueNode* next;
};

// ======================================================================
// BLOK QUALITY CONTROL: Validasi Input Terpusat (Tugas: Andika - 25.11.6307)
// ======================================================================

// Validasi input angka bulat (Integer)
int inputInt(string prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(10000, '\n'); // Bersihkan buffer newline
            return val;
        }
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Error: Input harus berupa angka bulat (integer)! Silakan coba lagi.\n";
    }
}

// Validasi input angka desimal (Float) dengan batas rentang
float inputFloat(string prompt, float minVal, float maxVal) {
    float val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= minVal && val <= maxVal) {
            cin.ignore(10000, '\n'); // Bersihkan buffer newline
            return val;
        }
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Error: Input harus angka desimal antara " << minVal << " dan " << maxVal << "! Silakan coba lagi.\n";
    }
}

// Validasi input NIM (String bertitik, hanya menerima angka dan titik)
string inputNIM(string prompt) {
    string val;
    while (true) {
        cout << prompt;
        getline(cin, val);
        if (val.empty()) {
            getline(cin, val);
        }
        
        bool valid = true;
        if (val.empty()) {
            valid = false;
        } else {
            for (size_t i = 0; i < val.length(); i++) {
                if (!isdigit(val[i]) && val[i] != '.') {
                    valid = false;
                    break;
                }
            }
        }
        
        if (valid) {
            return val;
        }
        cout << "Error: NIM hanya boleh berisi angka dan tanda titik! Silakan coba lagi.\n";
    }
}

// Validasi input kalimat/kata (String) terhindar dari sisa buffer \n
string inputString(string prompt) {
    string val;
    cout << prompt;
    getline(cin, val);
    if (val.empty()) {
        getline(cin, val);
    }
    return val;
}


// ======================================================================
// KELAS UTAMA: ENKAPSULASI, PERSISTENSI, & MANAJEMEN MEMORI (OOP)
// ======================================================================

class ManajemenMahasiswa {
private:
    // ======================================================================
    // BLOK LINKED LIST CORE: Node Pointers (Tugas: Haikal Abrar - 25.11.6328)
    // ======================================================================
    Mahasiswa* head;
    Mahasiswa* tail;
    
    // ======================================================================
    // BLOK STACK & QUEUE MEMORY: Node Pointers (Tugas: Dwi Risqi - 25.11.6553)
    // ======================================================================
    Mahasiswa* undoStack;
    QueueNode* qFront;
    QueueNode* qRear;

    // Helper: Hitung nilai akhir dan grade (Tugas: Andika Rahman Saula - 25.11.6307)
    void hitungNilai(Mahasiswa* m) {
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

    // Helper: Mencari node mahasiswa berdasarkan NIM (Tugas: Haikal Abrar - 25.11.6328)
    Mahasiswa* cariNodeMhs(string nim) {
        Mahasiswa* current = head;
        while (current != NULL) {
            if (current->nim == nim) {
                return current;
            }
            current = current->next;
        }
        return NULL;
    }

    // Helper: Membuat node mahasiswa baru (Tugas: Haikal Abrar - 25.11.6328)
    Mahasiswa* buatNodeBaru(string nim, string nama, float kuis[]) {
        Mahasiswa* baru = new Mahasiswa();
        baru->nim = nim;
        baru->nama = nama;
        baru->nilaiKuis[0] = kuis[0];
        baru->nilaiKuis[1] = kuis[1];
        baru->nilaiKuis[2] = kuis[2];
        hitungNilai(baru);
        baru->next = NULL;
        return baru;
    }

    // Helper: Menyisipkan node di akhir list utama (Tugas: Haikal Abrar - 25.11.6328)
    void insertLastList(Mahasiswa* m) {
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
    void pushUndo(Mahasiswa* m) {
        m->next = undoStack;
        undoStack = m;
    }

    Mahasiswa* popUndo() {
        if (undoStack == NULL) {
            return NULL;
        }
        Mahasiswa* temp = undoStack;
        undoStack = undoStack->next;
        temp->next = NULL;
        return temp;
    }

    // ======================================================================
    // BLOK QUEUE: Operasi Queue untuk Antrean Revisi (Tugas: Dwi Risqi - 25.11.6553)
    // ======================================================================
    void enqueueRevisi(string nim) {
        QueueNode* baru = new QueueNode();
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
        QueueNode* temp = qFront;
        nim = temp->nim;
        qFront = qFront->next;
        if (qFront == NULL) {
            qRear = NULL;
        }
        delete temp;
        return true;
    }

    bool isMhsInQueue(string nim) {
        QueueNode* current = qFront;
        while (current != NULL) {
            if (current->nim == nim) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // ======================================================================
    // BLOK INTEGRASI SISTEM: Penyimpanan File Aman (Tugas: Haikal Abrar - 25.11.6328)
    // ======================================================================
    void simpanKeFile() {
        ofstream file("mahasiswa.tmp");
        if (!file.is_open()) return;
        Mahasiswa* current = head;
        while (current != NULL) {
            file << current->nim << ";" << current->nama << ";" 
                 << current->nilaiKuis[0] << ";" << current->nilaiKuis[1] << ";" << current->nilaiKuis[2] << "\n";
            current = current->next;
        }
        file.close();
        remove("mahasiswa.txt");
        rename("mahasiswa.tmp", "mahasiswa.txt");
    }

    void muatDariFile() {
        ifstream file("mahasiswa.txt");
        if (!file.is_open()) return;
        string nimStr, nama, temp;
        float kuis[3];
        int count = 0;
        while (getline(file, nimStr, ';')) {
            if (nimStr.empty() || nimStr == "\n" || nimStr == "\r") continue;
            // Langsung menggunakan nimStr bertipe string tanpa atoi
            getline(file, nama, ';');
            getline(file, temp, ';'); kuis[0] = atof(temp.c_str());
            getline(file, temp, ';'); kuis[1] = atof(temp.c_str());
            getline(file, temp, '\n'); kuis[2] = atof(temp.c_str());
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
        Mahasiswa* currentMhs = head;
        while (currentMhs != NULL) {
            Mahasiswa* temp = currentMhs;
            currentMhs = currentMhs->next;
            delete temp;
        }
        head = NULL;
        tail = NULL;

        Mahasiswa* currentUndo = undoStack;
        while (currentUndo != NULL) {
            Mahasiswa* temp = currentUndo;
            currentUndo = currentUndo->next;
            delete temp;
        }
        undoStack = NULL;

        QueueNode* currentQ = qFront;
        while (currentQ != NULL) {
            QueueNode* temp = currentQ;
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
    ~ManajemenMahasiswa() {
        bersihkanMemori();
    }

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
        cout << "\n====================================================================\n"
             << "                      DAFTAR NILAI MAHASISWA                        \n"
             << "====================================================================\n"
             << left << setw(12) << "NIM" << setw(20) << "Nama" << right << setw(8) << "K1" << setw(8) << "K2" << setw(8) << "K3" << setw(10) << "Akhir" << setw(6) << "Grade" << "\n"
             << "--------------------------------------------------------------------\n";
        Mahasiswa* current = head;
        while (current != NULL) {
            string displayName = current->nama;
            if (displayName.length() > 17) {
                displayName = displayName.substr(0, 14) + "...";
            }
            cout << left << setw(12) << current->nim 
                 << setw(20) << displayName
                 << right << setw(8) << fixed << setprecision(1) << current->nilaiKuis[0] << setw(8) << current->nilaiKuis[1] << setw(8) << current->nilaiKuis[2]
                 << setw(10) << current->nilaiAkhir << setw(6) << current->grade << "\n";
            current = current->next;
        }
        cout << "====================================================================\n";
    }

    void editData() {
        cout << "\n-----------------------------------------\n"
             << "           EDIT DATA MAHASISWA           \n"
             << "-----------------------------------------\n";
        string nim = inputNIM("Masukkan NIM: ");
        Mahasiswa* m = cariNodeMhs(nim);
        if (m == NULL) {
            cout << "Mahasiswa dengan NIM " << nim << " tidak ditemukan.\n";
            return;
        }
        cout << "Nama Saat Ini: " << m->nama << "\n\n";
        m->nama = inputString("Masukkan Nama Baru: ");
        m->nilaiKuis[0] = inputFloat("Masukkan Nilai Kuis 1 Baru (0-100): ", 0.0, 100.0);
        m->nilaiKuis[1] = inputFloat("Masukkan Nilai Kuis 2 Baru (0-100): ", 0.0, 100.0);
        m->nilaiKuis[2] = inputFloat("Masukkan Nilai Kuis 3 Baru (0-100): ", 0.0, 100.0);
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
        Mahasiswa* current = head;
        Mahasiswa* prev = NULL;
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
        cout << "\nData \"" << current->nama << "\" berhasil dihapus! (Bisa dibatalkan dengan Undo Hapus)\n";
    }

    // Stack: Undo Hapus (Tugas: Dwi Risqi Ramadhan - 25.11.6553)
    void undoHapus() {
        cout << "\n-----------------------------------------\n"
             << "               UNDO HAPUS                \n"
             << "-----------------------------------------\n";
        Mahasiswa* m = popUndo();
        if (m == NULL) {
            cout << "Tidak ada data untuk di-undo (Stack kosong).\n";
            return;
        }
        if (cariNodeMhs(m->nim) != NULL) {
            cout << "Gagal memulihkan. NIM " << m->nim << " sudah terdaftar kembali.\n";
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
                string nim = inputNIM("Masukkan NIM mahasiswa yang mengajukan revisi: ");
                if (cariNodeMhs(nim) == NULL) cout << "Mahasiswa tidak ditemukan.\n";
                else if (isMhsInQueue(nim)) cout << "Mahasiswa sudah berada di antrean.\n";
                else { enqueueRevisi(nim); cout << "Berhasil ditambahkan ke antrean.\n"; }
            } 
            else if (sub == 2) {
                string nim;
                if (dequeueRevisi(nim)) {
                    Mahasiswa* m = cariNodeMhs(nim);
                    if (m != NULL) {
                        cout << "\nMemproses revisi: " << m->nama << "\n";
                        m->nilaiKuis[0] = inputFloat("Masukkan Nilai Kuis 1 Baru (0-100): ", 0.0, 100.0);
                        m->nilaiKuis[1] = inputFloat("Masukkan Nilai Kuis 2 Baru (0-100): ", 0.0, 100.0);
                        m->nilaiKuis[2] = inputFloat("Masukkan Nilai Kuis 3 Baru (0-100): ", 0.0, 100.0);
                        hitungNilai(m);
                        simpanKeFile();
                        cout << "Revisi selesai diproses.\n";
                    } else cout << "Mahasiswa dalam antrean sudah dihapus.\n";
                } else cout << "Antrean revisi kosong.\n";
            } 
            else if (sub == 3) {
                if (qFront == NULL) cout << "Antrean kosong.\n";
                else {
                    int pos = 1;
                    QueueNode* current = qFront;
                    while (current != NULL) {
                        Mahasiswa* m = cariNodeMhs(current->nim);
                        cout << pos << ". NIM: " << current->nim << " | Nama: " << (m ? m->nama : "[Data Terhapus]") << "\n";
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
        Mahasiswa* m = cariNodeMhs(nim);
        if (m != NULL) {
            cout << "\nData Ditemukan!\n"
                 << "NIM          : " << m->nim << "\n"
                 << "Nama         : " << m->nama << "\n"
                 << "Nilai Kuis   : " << m->nilaiKuis[0] << ", " << m->nilaiKuis[1] << ", " << m->nilaiKuis[2] << "\n"
                 << "Nilai Akhir  : " << fixed << setprecision(2) << m->nilaiAkhir << "\n"
                 << "Grade        : " << m->grade << "\n";
        } else cout << "Mahasiswa tidak ditemukan.\n";
    }

    // ======================================================================
    // BLOK SORTING: Pengurutan Data Ranking (Tugas: Razaan Azzaryan Bilhaq - 25.11.6309)
    // ======================================================================
    
    // Bubble Sort manual pada salinan Linked List (Murni memanipulasi node & pointer)
    void rankingNilai() {
        if (head == NULL) {
            cout << "Data kosong.\n";
            return;
        }
        
        // 1. Menyalin Linked List ke list temporary baru untuk diurutkan
        Mahasiswa* tempHead = NULL;
        Mahasiswa* tempTail = NULL;
        
        Mahasiswa* current = head;
        while (current != NULL) {
            Mahasiswa* baru = new Mahasiswa();
            baru->nim = current->nim;
            baru->nama = current->nama;
            baru->nilaiKuis[0] = current->nilaiKuis[0];
            baru->nilaiKuis[1] = current->nilaiKuis[1];
            baru->nilaiKuis[2] = current->nilaiKuis[2];
            baru->nilaiAkhir = current->nilaiAkhir;
            baru->grade = current->grade;
            baru->next = NULL;
            
            if (tempHead == NULL) {
                tempHead = baru;
                tempTail = baru;
            } else {
                tempTail->next = baru;
                tempTail = baru;
            }
            current = current->next;
        }
        
        // 2. Bubble Sort manual pada Linked List temporary dengan cara menukar data
        bool swapped;
        do {
            swapped = false;
            Mahasiswa* c = tempHead;
            while (c != NULL && c->next != NULL) {
                if (c->nilaiAkhir < c->next->nilaiAkhir) {
                    // Tukar NIM (String)
                    string tempNIM = c->nim;
                    c->nim = c->next->nim;
                    c->next->nim = tempNIM;

                    // Tukar Nama
                    string tempNama = c->nama;
                    c->nama = c->next->nama;
                    c->next->nama = tempNama;

                    // Tukar Nilai Kuis
                    for (int i = 0; i < 3; i++) {
                        float tempKuis = c->nilaiKuis[i];
                        c->nilaiKuis[i] = c->next->nilaiKuis[i];
                        c->next->nilaiKuis[i] = tempKuis;
                    }

                    // Tukar Nilai Akhir
                    float tempAkhir = c->nilaiAkhir;
                    c->nilaiAkhir = c->next->nilaiAkhir;
                    c->next->nilaiAkhir = tempAkhir;

                    // Tukar Grade
                    char tempGrade = c->grade;
                    c->grade = c->next->grade;
                    c->next->grade = tempGrade;

                    swapped = true;
                }
                c = c->next;
            }
        } while (swapped);
        
        // 3. Menampilkan hasil ranking terurut dari Linked List temporary
        cout << "\n====================================================================\n"
             << "                   RANKING NILAI MAHASISWA (BUBBLE SORT)            \n"
             << "====================================================================\n"
             << left << setw(8) << "Rank" << setw(12) << "NIM" << setw(20) << "Nama" << right << setw(8) << "K1" << setw(8) << "K2" << setw(8) << "K3" << setw(10) << "Akhir" << setw(6) << "Grade" << "\n"
             << "--------------------------------------------------------------------\n";
        
        int rank = 1;
        Mahasiswa* c = tempHead;
        while (c != NULL) {
            string displayName = c->nama;
            if (displayName.length() > 17) {
                displayName = displayName.substr(0, 14) + "...";
            }
            cout << left << setw(8) << rank << setw(12) << c->nim 
                 << setw(20) << displayName
                 << right << setw(8) << fixed << setprecision(1) << c->nilaiKuis[0] << setw(8) << c->nilaiKuis[1] << setw(8) << c->nilaiKuis[2]
                 << setw(10) << c->nilaiAkhir << setw(6) << c->grade << "\n";
            c = c->next;
            rank++;
        }
        cout << "====================================================================\n";
        
        // 4. Membersihkan memori Linked List temporary
        while (tempHead != NULL) {
            Mahasiswa* temp = tempHead;
            tempHead = tempHead->next;
            delete temp;
        }
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
            case 1: app.tambahData(); break;
            case 2: app.tampilData(); break;
            case 3: app.editData(); break;
            case 4: app.hapusData(); break;
            case 5: app.undoHapus(); break;
            case 6: app.antreanRevisi(); break;
            case 7: app.cariNIM(); break;
            case 8: app.rankingNilai(); break;
            case 0: cout << "Terima kasih telah menggunakan aplikasi ini.\n"; break;
            default: cout << "Pilihan tidak tersedia. Silakan coba lagi.\n"; break;
        }
    } while (pilihan != 0);

    return 0;
}
