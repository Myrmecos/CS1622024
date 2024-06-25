/*
 * Word count application with one thread per input file.
 *
 * You may modify this file in any way you like, and are expected to modify it.
 * Your solution must read each input file from a separate thread. We encourage
 * you to make as few changes as necessary.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "word_count.h"
#include "word_helpers.h"
#define SIZE_LIST 64

/*
 * main - handle command line, spawning one thread per file.
 */

typedef struct {
  word_count_list_t* word_counts;
  char* myfile;
}list_and_file_t;

void* thread_func(void* list_and_file) {
  list_and_file_t* laf = (list_and_file_t*) list_and_file;
  fprintf(stdout, "processing file %s\n", laf->myfile);
  FILE* myfile = fopen(laf->myfile, "r");
  count_words(laf->word_counts, myfile);
  pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
  /* Create the empty data structure. */
  word_count_list_t word_counts;
  init_words(&word_counts);
  //fprintf(stdout, "hello");
  if (argc <= 1) {
    /* Process stdin in a single thread. */
    //fprintf(stdout, "hello this is the case when argc<1\n");
    count_words(&word_counts, stdin);
  } else {
    //FILE * myfile = fopen(argv[1], "r");
    //count_words(&word_counts, myfile);
    /*list_and_file_t * laf = malloc(sizeof(laf));
    laf->myfile = malloc(sizeof(char)*SIZE_LIST);
    strcpy(laf->myfile, argv[1]);
    laf->word_counts = &word_counts;
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, (void*)laf);
    pthread_join(thread, NULL);*/

    pthread_t* threads = malloc(sizeof(pthread_t) * argc);

    for (int i = 0; i < argc-1; i++) {
      //fprintf(stdout, "opening file %s", argv[i+1]);
      list_and_file_t * laf = malloc(sizeof(laf));
      laf->myfile = malloc(sizeof(char)*SIZE_LIST);
      strcpy(laf->myfile, argv[i+1]);
      laf->word_counts = &word_counts;
      pthread_create(&(threads[i]), NULL, thread_func, (void*)laf);
    }

    for (int i = 0; i < argc-1; i++) {
      pthread_join(threads[i], NULL);
    }
  }

  /* Output final result of all threads' work. */
  //wordcount_sort(&word_counts, less_count);
  fprint_words(&word_counts, stdout);
  return 0;
}
