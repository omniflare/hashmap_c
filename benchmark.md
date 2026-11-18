# Hashmap Benchmark Results

Performance comparison between Separate Chaining and Double Hashing.

Run using `make benchmark`.

All timings are in seconds. Results depend on the machine and compiler configuration.

| Strategy | Entries | Insertion | Successful Search | Unsuccessful Search | Deletion |
|---|---:|---:|---:|---:|---:|
| Separate Chaining | 10000 | 0.0018 | 0.0003 | 0.0012 | 0.0010 |
| Double Hashing | 10000 | 0.0037 | 0.0005 | 0.0020 | 0.0016 |
| Separate Chaining | 100000 | 0.0118 | 0.0028 | 0.0103 | 0.0083 |
| Double Hashing | 100000 | 0.0233 | 0.0064 | 0.0177 | 0.0129 |
| Separate Chaining | 500000 | 0.0418 | 0.0127 | 0.0444 | 0.0359 |
| Double Hashing | 500000 | 0.1115 | 0.0388 | 0.1185 | 0.0623 |
| Separate Chaining | 1000000 | 0.0875 | 0.0255 | 0.0861 | 0.0704 |
| Double Hashing | 1000000 | 0.2601 | 0.0886 | 0.2380 | 0.1338 |
