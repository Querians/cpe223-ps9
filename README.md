# CacheLab
This is a lab 9 under **CPE223 : Computer Architecture**.

## Pre-Installation
You should have **c compiler** and **make** to build this project more easier.

We develop by using **MinGW-w64** and **GNU Make 4.3**. However, There are several ways to install them. Because we mainly use window, we use [choco](https://community.chocolatey.org/) to install these above by using command

```bash
choco install mingw
```

and

```bash
choco install make
```

>You can also use **Code Runner** extension in VScode to run instead of using make.

## Clone repository
```bash
git clone https://github.com/Querians/cpe223-ps9
```

## Usage
### FIFO ([cacheLab.c](cacheLab.c))
Compile
```bash
make build-fifo
make
```
Excute
```bash
./cacheLab
```
Clean
<br>*-must do after excute and want to run new excution file-*
```bash
make clean
```

### LRU ([cacheEC.c](cacheEC.c))
Compile
```bash
make build-lru
make
```
Excute
```bash
./cacheEC
```
Clean
<br>*-must do after excute and want to run new excution file-*
```bash
make clean
```

## Test Cases
To test our testcase, just change input [line 408 in file cacheLab.c] or [line 473 in file cacheEC.c] to each file in [test](./test) folder.

### Test Cases Description
1. Worse case that do not hit any cacheline by access address differ by 16 (0x10).
```c
0 1300 1
0 1310 1  //0x1300 + 0x10 
0 1320 1  //0x1310 + 0x10
0 1330 1
0 1340 1
0 1350 1
0 1360 1
0 1370 1
0 1380 1
```

2. Always access the same address, so hit cache everytime except the first time that get from DRAM.
```c
0 1300 99
0 1300 99
0 1300 99
0 1300 99
0 1300 99
0 1300 99
```

3. From the first line to the fourth line, access the address that differ by 16 in decimal (0x10).
```c
0 1300 1
0 1310 2
0 1320 3
0 1330 4
0 1300 5  // same as the first line
0 1340 6  // new address
```
> For FIFO, timestamp does not change every time that access. Finally, the first cache block is replaced by cache block from the sixth line request.

> For LRU, timestamp always change after access. The first cache block have just changed from the fifth line request, so the third cache block which have longest time access would be replaced by the sixth line request.

4. Case that found the cache to write in L2 instead of L1. 
```c
0 1300 1
0 1310 2
0 1320 3
0 1330 4
0 1340 5  // replace at the first cacheline
1 1300 1726  // hit at L2
```
Since the first cache was kicked out from L1 by request from line 5, the sixth line request that require to write the kicked cache block from L1 must access L2 and edit at L2. After edit at L2 and DRAM, the updated address must read back to L1 again.

5. Test the write function by write the address that differ by 4. Then test to write the address that not in cache.
```c
0 1300 0
1 1300 0    // L1 hit 1
1 1304 0    // same cache block with address 1300, L1 hit 2
1 1308 0    // same cache block with address 1300, L1 hit 3
1 130c 0    // same cache block with address 1300, L1 hit 4
0 1308 0    // same cache block with address 1300, L1 hit 5
1 1310 12   // write to DRAM
0 1310 12   // read to cache block and check that value have changed
```

6. To see the difference between FIFO and LRU.
```c
0 1300 1
0 1310 2
0 1320 3
0 1330 4
1 1300 100  // update timestamp in LRU but not in FIFO
0 1340 5
```
> For FIFO, the first request (0x98 set 0) must be the first kicked out cache.

> For LRU, the longest access must be kicked first which is (0x99 set 0) instead of (0x98 set 0) that have write in nearest time.

## Moreover
We have seperated the file into 2 repositories in replit.com to test it more easier.
- [CacheLab - FIFO](https://replit.com/@qimZithe/CPE223-PS9-CacheLab)
- [CacheEC - LRU](https://replit.com/@qimZithe/CPE223-PS9-CacheEC)

## Members

FirstName | LastName | ID | Github
--- | --- | --- | ---
Kanyaluck | Chimchome | 64070501003 | [Parefair](https://github.com/Parefair)
Boonyarit | Samran | 64070501028 | [QuartzQw](https://github.com/QuartzQw)
Warisara | Patib | 64070501044 | [Cocoa2304](https://github.com/Cocoa2304)
Chanidapa | Chanama | 64070501090 | [Qandle](https://github.com/Qandle)
Nontawat | Kunlayawuttipong | 64070501093 | [nontaxim](https://github.com/nontaxim)
