# Sistem Manajemen Perpustakaan (CLI) — Bahasa C

Project UAS Lab Pemrograman Kelas A — Informatika FMIPA Universitas Syiah Kuala (2026).

## Anggota Kelompok
| Nama Lengkap | NPM |
|--------------|-----|
| Almira Carissa Putri | 260810701100041 |
| Fairuz Phoenna Ananda | 260810701100008 |
| Muhammad Athar Firdaus | 260810701100039 |
| Siti Masitah | 260810701100022 |


## Struktur Berkas
```
Perpustakaan-CLI/
├── Makefile
├── README.md
├── data/
│   ├── Books.txt        # dataset ASLI (read-only, tidak pernah diubah program)
│   ├── katalog.txt      # (dibuat otomatis) inventaris operasional = Books.txt + stok
│   ├── user.txt         # (dibuat otomatis) akun: username password role
│   └── peminjaman.txt   # (dibuat otomatis) id username ocaid DD-MM-YYYY STATUS
└── src/
    ├── main.c           # alur utama: init -> login -> menu per role -> logout
    ├── config.h         # konstanta (path berkas, ukuran field, stok default)
    ├── util.c/.h        # input aman, validasi, perbandingan string, tanggal
    ├── book.c/.h        # struct Book, daftar dinamis, parser dataset, load/save katalog
    ├── user.c/.h        # struct User, load/save akun, autentikasi
    ├── loan.c/.h        # struct Loan, load/save transaksi, logika pinjam & kembali
    ├── sorting.c/.h     # Bubble, Insertion, Selection Sort (manual, tanpa qsort)
    ├── ui.c/.h          # tabel berhalaman, menu sorting, katalog, pencarian
    ├── auth.c/.h        # login (interaktif / argumen) dan registrasi
    ├── menu_admin.c/.h  # menu & CRUD admin, laporan transaksi
    ├── menu_user.c/.h   # menu anggota
    └── app.c/.h         # pemuatan & pelepasan seluruh data (anti memory leak)
```

## Arsitektur Singkat
- **Data di memori**: `BookList`, `UserList`, `LoanList` — array of struct dinamis (`realloc`), dibebaskan di `app_free()`.
- **Dataset asli tidak diubah.** Saat pertama dijalankan, `Books.txt` dibaca lalu disalin ke
  `data/katalog.txt` dengan tambahan kolom **stok** (default 3 per judul). Seluruh CRUD,
  peminjaman, dan pengembalian ditulis ke `katalog.txt`, `user.txt`, `peminjaman.txt`.
- **Parser dataset** menangani field berpetik dua, tanda petik ganda di dalam judul (`""`), serta
  judul/pengarang satu kata yang tidak diberi petik.
- **Penyimpanan aman**: setiap perubahan menulis ke berkas `.tmp` lalu di-`rename`, sehingga berkas
  tidak rusak jika program terhenti saat menulis.
- **Sorting** bekerja pada array *pointer* ke buku (bukan memindahkan struct besar), sehingga
  data asli tidak berubah dan pengurutan 10.000 data tetap cepat. Atribut: ID, judul, tahun,
  jumlah halaman, stok; arah naik/turun.

## Kompilasi & Menjalankan
Butuh `gcc` dan `make`. Jalankan dari folder utama repositori (agar folder `data/` terbaca):
```bash
make              # kompilasi -> ./perpustakaan
./perpustakaan    # login interaktif
./perpustakaan admin admin123    # login lewat Command Line Argument (opsional)
make debug        # build dengan AddressSanitizer (cek memory leak)
make reset-data   # hapus katalog/user/peminjaman hasil program (Books.txt aman)
```
Tanpa `make`:
```bash
gcc -std=c99 -Wall -Wextra -pedantic -O2 -o perpustakaan src/*.c
```

## Akun Bawaan
| Username | Password  | Role  |
|----------|-----------|-------|
| admin    | admin123  | admin |
| user1    | user123   | user  |
| user2    | user123   | user  |

Anggota baru dapat mendaftar lewat menu **Registrasi** di menu utama.

## Fitur
**Admin**: lihat katalog (+sorting), cari buku, tambah/ubah/hapus buku, laporan seluruh transaksi.
Buku yang masih dipinjam tidak bisa dihapus; stok tidak boleh negatif.

**User**: lihat katalog buku tersedia (+sorting), cari buku (ID / judul), pinjam (stok > 0, stok otomatis -1),
kembalikan (stok +1, status `KEMBALI`), daftar pinjaman aktif, riwayat pribadi.

Tampilan tabel dibagi per halaman (20 baris): `N` berikutnya, `P` sebelumnya, angka untuk lompat, `Q` selesai.
Disarankan lebar terminal ≥ 125 karakter.