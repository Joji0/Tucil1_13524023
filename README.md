# Queens LinkedIn Game Solver

**Tugas Kecil 1 — IF2211 Strategi Algoritma**  
**Semester II Tahun Ajaran 2025/2026**

Program untuk menyelesaikan permainan Queens dari LinkedIn menggunakan algoritma Brute Force, dengan antarmuka grafis (GUI) berbasis SFML.

---

## Deskripsi Program

Permainan Queens menantang pemain untuk menempatkan N queens pada papan N×M yang terbagi dalam N region warna, dengan aturan:
- Tepat satu queen per region warna
- Tepat satu queen per baris dan satu queen per kolom
- Tidak ada dua queen yang bersentuhan secara horizontal, vertikal, maupun diagonal

Program ini menyediakan:
- **Brute Force** — mencoba seluruh $M^N$ kemungkinan penempatan secara sistematis
- **GUI interaktif** — visualisasi real-time proses pencarian solusi
- **Export PNG** — menyimpan solusi akhir sebagai gambar

---

## Kebutuhan Sistem

| Komponen | Keterangan |
|----------|------------|
| Compiler | **g++** dengan dukungan C++17 |
| Build tool | **GNU Make** |
| Library | **SFML 3.0** (Simple and Fast Multimedia Library) |

---

## Instalasi & Kompilasi

### Windows

SFML 3.0 sudah disertakan dalam folder `lib/SFML/`. Tidak perlu instalasi tambahan.

```bash
make          # kompilasi
make run      # kompilasi & jalankan
make clean    # bersihkan hasil build
```

Makefile secara otomatis menyalin file DLL dari `lib/SFML/bin/` ke direktori kerja. Executable yang dihasilkan: `main.exe`.

### macOS

Instal SFML melalui Homebrew:

```bash
brew install sfml
```

Kemudian build:

```bash
make          # kompilasi
make run      # kompilasi & jalankan
make clean    # bersihkan hasil build
```

Makefile mendeteksi apakah SFML terpasang di `/opt/homebrew` (Apple Silicon) atau `/usr/local` (Intel). Executable yang dihasilkan: `main`.

### Linux

Instal SFML melalui apt:

```bash
sudo apt-get install libsfml-dev
```

Kemudian build:

```bash
make          # kompilasi
make run      # kompilasi & jalankan
make clean    # bersihkan hasil build
```

Makefile menggunakan library SFML dari path sistem default. Executable yang dihasilkan: `main`.

---

## Cara Menjalankan

```bash
# Via makefile
make run

# Atau jalankan executable langsung
./main          # macOS / Linux
main.exe        # Windows
```

### Panduan Penggunaan GUI:

1. **BROWSE** — pilih file `.txt` berisi konfigurasi papan Queens
2. **Pilih Algoritma:**
   - `Brute Force` — mencoba semua kemungkinan (exhaustive)
3. **Atur Kecepatan** — geser slider di panel kiri:
   - *TURBO*: delay 0–50 ms
   - *FAST*: delay 50–5000 ms
   - *SLOW*: delay > 5000 ms
4. **SOLVE** — mulai pencarian solusi
5. **STOP** — hentikan pencarian yang sedang berjalan
6. **SAVE TXT** — simpan solusi sebagai file `.txt` (dengan waktu dan iterasi)
7. **SAVE IMG** — simpan solusi sebagai file PNG

### Format Input File (`.txt`):

```
AAABBCCCD
ABBBBCECD
ABBBDCECD
AAABDCCCD
BBBBDDDDD
FGGGDDHDD
FGIGDDHDD
FGIGDDHDD
FGGGDDHHH
```

- Setiap karakter merepresentasikan region warna (huruf kapital A–Z)
- Semua baris harus memiliki panjang yang sama
- Jumlah region warna unik harus sama dengan jumlah baris (N)

---

## Struktur Folder

```
Tucil1_13524023/
├── src/
│   └── main.cpp              
├── test/
│   ├── in1.txt               
│   ├── in2.txt               
│   ├── in3.txt               
│   ├── in4.txt               
│   ├── in5.txt               
│   ├── solusi_in1.txt        
│   ├── solusi_in2.txt        
│   ├── solusi_in3.txt        
│   ├── solusi_in4.txt        
│   └── solusi_in5.txt        
├── doc/
│   └── latex/               
├── lib/
│   └── SFML/                 
├── makefile                  
├── README.md                 
└── queen.png                 
```

---

## Identitas Pembuat

| | |
|---|---|
| **Nama** | Jonathan Kris Wicaksono |
| **NIM** | 13524023 |
| **Kelas** | K01 |
| **Program Studi** | Teknik Informatika |
| **Institusi** | Institut Teknologi Bandung (ITB) |
