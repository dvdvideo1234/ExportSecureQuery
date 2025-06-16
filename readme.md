# Description
Insert a record in a table only if the record exists. Converts insert-into table_name(...) values(...) into secure-inserts.

# Configuration
The program parameters and the table-specific options are located in the [settings folder][ref-settings]
```
i: Do not put the column in the insert list
v: Do not put the column in the values list
w: Do not put the column in the where clause list
```
# Legend
The program will process your file line-by-line and show you a progress bar ( right in `%` )
```
Processing...
         1x         2x         3x         4i         5v         6x   19.3548%
         7i         8v         9x        10i        11v        12x   38.7097%
        13i        14v        15x        16i        17v        18x   58.0645%
i: Insert keyword was processed on this line
v: Values keyword was processed on this line
x: Nothing has been found on this line
```

# How to use
Export some stuff from PL/SQL Developer as *.sql then drop the file onto the executable.

# Compiling
For all of my projects I aim to use [Code::Blocks][ref-code-block] as it is highly accessible and customizable.
I do not like Visual Studio and I never will It is too damn weird for path different than `C:\`.

[ref-code-block]: https://www.codeblocks.org/
[ref-settings]: https://github.com/dvdvideo1234/ExportSecureQuery/tree/main/settings
