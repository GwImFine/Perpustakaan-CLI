#ifndef CONFIG_H
#define CONFIG_H

/* Lokasi berkas data (relatif terhadap direktori tempat program dijalankan) */
#define FILE_BOOKS_MASTER "data/Books.txt"      /* dataset asli (HANYA DIBACA) */
#define FILE_CATALOG      "data/katalog.txt"    /* inventaris operasional (+stok) */
#define FILE_USERS        "data/user.txt"
#define FILE_LOANS        "data/peminjaman.txt"

/* Batas ukuran field (sesuai spesifikasi soal & hasil analisis dataset) */
#define LEN_USERNAME 32
#define LEN_PASSWORD 32
#define LEN_ROLE     12
#define LEN_OCAID    64
#define LEN_DATE     20
#define LEN_STATUS   20
#define LEN_TITLE    1200
#define LEN_AUTHOR   480
#define LEN_OLID     32
#define LEN_URL      128

#define LINE_BUFFER   8192
#define DEFAULT_STOCK 3     /* stok awal tiap judul saat dataset diimpor */
#define PAGE_SIZE     20    /* baris per halaman pada tampilan tabel     */

#define ROLE_ADMIN "admin"
#define ROLE_USER  "user"
#define STATUS_BORROWED "DIPINJAM"
#define STATUS_RETURNED "KEMBALI"

#endif