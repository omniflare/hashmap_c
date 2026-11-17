
CC = clang

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g \
         -D_POSIX_C_SOURCE=200809L

INCLUDES = -Isrc

SRC = src/hash_table.c src/prime.c

TARGET = hashmap
BENCHMARK_BIN = benchmark_bin

all: $(TARGET)

$(TARGET): src/main.c $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(BENCHMARK_BIN): benchmarks/benchmark.c $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

run: $(TARGET)
	./$(TARGET)

benchmark: $(BENCHMARK_BIN)
	./$(BENCHMARK_BIN)

clean:
	rm -f $(TARGET) $(BENCHMARK_BIN)

.PHONY: all run benchmark clean
