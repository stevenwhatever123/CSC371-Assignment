Compiler: gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)

I used Clion to do this assignment, I have to move the datasets folder
into cmake-build-debug in order to read data. Otherwise, it could not find the files.

# What is this coursework about?  
This is my attempt on CSC371-Advanced Object Oriented Programming module Assignment. I develop a tool in C++ that can read CSV and JSON files, some of which have been downloaded from StatsWales website.  

This tool read user arguments and will output a table based on the given arguments.  

For example:  
If we request data from the popden datasets, for the areas W06000011 and W06000010, between 1990 and 1993, the command is:  
$ ./bin/bethyw -d popden -a W06000011, W06000010 -y 1990-1993  

Where we would get the output:  
```
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
```

If we request data from the popden and trains datasets, for the area W06000011, with the rail and pop measure, between 2015 and 2018, the command is:  
$ ./bin/bethyw -d popden,trains -a W06000011 -m rail,pop -y 2015-2018  

with the output:
```
Swansea / Abertawe (W06000011)
Population (pop)
         2015          2016          2017          2018       Average       Diff.   %Diff. 
242316.000000 244462.000000 245480.000000 246466.000000 244681.000000 4150.000000 1.712640 

Rail passenger journeys (rail)
         2015          2016          2017          2018       Average        Diff.   %Diff. 
910878.000000 914448.000000 921736.000000 927841.000000 918725.750000 16963.000000 1.862269 

```

There is also a JSON output. For example:
$ ./bin/bethyw -d popden,trains -a W06000011 -m rail,pop -y 2015-2018 -j

```
{
  "W06000011":{
    "measures":{
      "pop":{
        "2015":242316.0,
        "2016":244462.0,
        "2017":245480.0,
        "2018":246466.0
      },
      "rail":{
        "2015":910878.0,
        "2016":914448.0,
        "2017":921736.0,
        "2018":927841.0
      }
    },
    "names":{
      "cym":"Abertawe",
      "eng":"Swansea"
    }
  }
}
```
