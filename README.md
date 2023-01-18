compile with:
gcc -std=c99  -Werror -Wextra -pedantic -Wno-unused-parameter  main.c utility.c node.c buckets.c

start program by providing a valid filename as parameter. e.g. "./a.out test.txt"

once the programm successfully started, a list of hash-buckets will be printed out.

the user will be asked to select buckets. select buckets by entering an integer between 0-36
confirm the selection by pressing -1
3 new files will be created. 1 file will contain all separators and all words that are within the selected buckets. 1 file will contain all separators and all words that are NOT within the selected buckets. the third file will only contain the hash of each word, separated by a space character.

afterwards the user will be prompted to enter words that should be appended to the hash-file (as hash).
end program with -1
