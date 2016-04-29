# Duplicate code checker

A simple duplicate code checker based on [Levenshtein distance (Editing Distance)](https://en.wikipedia.org/wiki/Levenshtein_distance).

For the comparison between each pair of files S and T, the algorithm runs with a time complexity of $O(|S| \times |T|)$ and a memory complexity of $O(|T|)$, where $|S|$ and $|T|$ denotes the lengths of two files respectively.

## Usage

```
make clean
make
./dupcheck [dirname]
```

An example directory has been given as `testdata/`. The command `./dupcheck testdata` will generate a similarity score matrix in `result.txt` and another id-filename mapper in `name.txt`.

## TODO features

* Winnowing
* consine similarity
