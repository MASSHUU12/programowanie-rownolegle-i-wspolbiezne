# Programowanie Równoległe i Współbieżne

> g++ requires 'gcc-c++' package on Fedora.

## Lab 1

```sh
g++ -std=c++11 -pthread ./lab1/main.cpp
```

### References

- https://en.cppreference.com/w/cpp/chrono
- https://cplusplus.com/reference/iomanip/setprecision/
- https://solarianprogrammer.com/2011/12/16/cpp-11-thread-tutorial/
- https://solarianprogrammer.com/2012/02/27/cpp-11-thread-tutorial-part-2/

## Lab 2

```sh
./build.sh && ./run.sh
```

### References

- https://mpalkowski.zut.edu.pl/lab1_odm.pdf
- https://solarianprogrammer.com/2011/12/16/cpp-11-thread-tutorial/
- https://en.cppreference.com/w/cpp/chrono
- https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/
- https://www.geeksforgeeks.org/2d-vector-in-cpp-with-user-defined-size/
- https://thispointer.com/c-11-multithreading-part-1-three-different-ways-to-create-threads/

## Lab 3

```sh
./build.sh && ./run.sh
```

### References

- https://mpalkowski.zut.edu.pl/lab2_odm.pdf
- https://rosettacode.org/wiki/Mandelbrot_set#PPM_non_interactive

## ~Lab 4~

```sh
./build.sh
```

### References

- https://mpalkowski.zut.edu.pl/lab3_odm.pdf
- https://www.geeksforgeeks.org/socket-programming-cc/
- https://www.tutorialspoint.com/how-to-output-colored-text-to-a-linux-terminal

## Exercise 3

```txt
Tablica integerow. Sciana -1.
Watki przechodzac przez korytarz wpisuja tam wartosc swoj tid.
Poczatkowo sciany maja wartosc 0.
Watek sprawdza czy moze isc kierunkie lewo prawo gora dol.
Wartosc >0 tzn ze juz tam ktos byl.
Moze zajac tylko korytarz z 0. Skrzyzowanie.
Watek wybiera jedna droge a dla pozostalych tworzy nowe watki.
Sprawdzanie  i wpisywanie wartosci robimy w sekcji krytycznej
(tablicy locków, mutexow). Tablice zapisujecie do PPM i wizualizujecie.
(powiekszyc tablice do lepszego odczytu - 1 x 9).
Labirynt generowany samodzielnie.
Numery watkow potomkow trzymamy we wektorze lokalnym.
```

```sh
./build.sh && ./maze
```

## Exercise 4

```txt
OpenMP

Mnożenie macierzy
Przeimplementować z zad 1 do biblioteki OpenMP z transpozycją włącznie.
Przełączniki kompilacji -lgomp -fopenmp -O3.
Wykorzystać
#pragma omp parallel
#pragma omp for
omp_get_wtime()

Dokumentacja: https://hpc-tutorials.llnl.gov/openmp/

Wzbogacić implementację o 3 mechanizm - blokowanie (tiling)
Nie przeliczać jednego elelementu, lecz od razu cały blok  np. 4x4, 8x8, 16x16, 32x32.
Porównać wyniki czasowe z poprzednimi.
```

Mnożenie zwykłe, z transpozycją i blokowe.

```sh
make
```

## Exercies 5

```txt
Labirynt w OpenMP.
```

```sh
make
```

## Exercise 6

```txt
https://mpalkowski.zut.edu.pl/lab_ulam.pdf
```

```sh
make && make png
```

## Exercise 7

```txt
do wyboru
wyklad 10 lib 11
https://mpalkowski.zut.edu.pl/
```

```sh
TODO
```
