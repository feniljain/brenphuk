#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "benchmark.h"
#include "interpreter.h"
#include "debug.h"

double benchmark_results[8][2]; // 7 commands, 1st column for count, 2nd for
                                // exec avg time

void print_benchmark_results(void) {
  for (int i = 0; i < 8; i++) {
    DBG_PRINTF("%d command executed %f times with %f total exec time", i,
               benchmark_results[i][0], benchmark_results[i][1]);
  }
}

typedef struct {
  char *prog;
  int len;
} ProgStr;

ProgStr read_file(char *file_path) {
  FILE *fptr;

  int idx = 0;
  char *buf = NULL, *prog = (char *)malloc(1000000);
  assert(prog != NULL);

  size_t linecap = 0;
  ssize_t linelen;

  fptr = fopen(file_path, "r");
  assert(fptr != NULL);

  while ((linelen = getline(&buf, &linecap, fptr)) > 0) {
    // - converting linelen is safe cause we have checked it's greater than zero already
		// - as we know memory regions are not overlapping, it's safe to use memcpy instead of memmove
    memcpy(prog + idx, buf, (size_t)linelen); 
    idx += (int)linelen;
  }

  ProgStr prog_str;
  prog_str.prog = prog;
  prog_str.len = idx;

  return prog_str;
}

int benchmark(int iterations) {
  clock_t start, end;
  double cpu_time_used, avg_cpu_time_used = 0.0;

  ProgStr prog_str = read_file("../programs/mandlebrot.b");
  DBG_PRINTF("prog: %s: %d", prog_str.prog, prog_str.len);

	for(int i = 0; i < iterations; i++) {
		#undef DBG_LOGS

		start = clock();

		exec(prog_str.prog, prog_str.len);

		end = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		avg_cpu_time_used += cpu_time_used;
		DBG_PRINTF("benchmark::program took: %f secs in %d iteration", cpu_time_used, i + 1);

		reset();
	}

	avg_cpu_time_used /= (double)iterations;
	DBG_PRINTF("benchmark::on average program took: %f secs", avg_cpu_time_used);

  reset();

  return 0;
}
