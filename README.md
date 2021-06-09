## Pipeline Implementation use c++ 
Computer Organization 

This program solve hazard.

### Introduction
stages &thinsp;&thinsp;&thinsp; | IF | ID | EXE | MEM | WB
---------|----|----|-----|-----|---


register&thinsp;&thinsp;| $0 | $1 | $2 | $3 | $4 | $5 | $6 | $7 | $8 | $9
---| ---|----|----|----|----|----|----|----|----|---
value|0 | 9 | 5 | 7 | 1 | 2 | 3 | 4 | 5 | 6 | 

memory | 0x00 | 0x04 | 0x08 |0x0C
-------|------|------|------|----
value | 5 | 9 | 4 | 8
### How to use
- Run `main.cpp`

- The Program loads instruction from `.txt` file in Input folder automatically.

- In the output folder, `main.cpp` creates `branchResult.txt`, `dataResult.txt`, `genResult.txt` and `loadResult.txt`
</br>

Input folder| Output folder
------------|-----------
Branchhazard.txt | branchResult.txt
Datahazard.txt | dataResult.txt
General.txt | genResult.txt
Lwhazard.txt | loadResult.txt


- The `.txt` in SampleOutput folder is what will be created after running `main.cpp` 
