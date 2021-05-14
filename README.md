Compiler: gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)

I used Clion to do this assignment, I have to move the datasets folder
into cmake-build-debug in order to read data. Otherwise, it could not find the files.

What is this coursework about?  
Develop a tool in C++ that can read CSV and JSON files, some of which have been downloaded from StatsWales website.  
This tool read user arguments and will output a table based on the given arguments.  

For example:  
If we request data from the popden datasets, for the areas W06000011 and W06000010, between 1990 and 1993, the command is:  
$ ./bin.bethyw -d popden -a W06000011, W06000010 -y 1990-1993  

Where we would get the output:  
---
Carmarthenshire / Sir Gaerfyrddin (W06000010)
Land area (area)
       1991        1992        1993     Average    Diff.   %Diff. 
2370.276200 2370.276200 2370.276200 2370.276200 0.000000 0.000000 

Population density (dens)
     1991      1992      1993   Average    Diff.   %Diff. 
71.605579 71.411509 71.407290 71.474793 -0.198289 -0.276918 

Population (pop)
         1991          1992          1993       Average      Diff.   %Diff. 
169725.000000 169265.000000 169255.000000 169415.000000 -470.000000 -0.276919 

Swansea / Abertawe (W06000011)
Land area (area)
      1991       1992       1993    Average    Diff.   %Diff. 
377.596400 377.596400 377.596400 377.596400 0.000000 0.000000 

Population density (dens)
      1991       1992       1993    Average    Diff.   %Diff. 
608.435356 608.083128 607.391914 607.970133 -1.043442 -0.171496 

Population (pop)
         1991          1992          1993       Average      Diff.   %Diff. 
229743.000000 229610.000000 229349.000000 229567.333333 -394.000000 -0.171496 
---
