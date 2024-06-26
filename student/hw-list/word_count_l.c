/*
 * Implementation of the word_count interface using Pintos lists.
 *
 * You may modify this file, and are expected to modify it.
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
#define WORD_LEN 64
#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_l.c"
#endif

#include "word_count.h"

//word_count_list_t is typedef of list in list.h

void init_words(word_count_list_t* wclist) {
  list_init(wclist);
}

size_t len_words(word_count_list_t* wclist) {
  /*return strlen of the word pointed at by wclist*/
  if (wclist == NULL) {
    return -1;
  }
  size_t len = 0;
  for (struct list_elem* e = list_begin(wclist); e != list_end(wclist); e = list_next(e)) {
    len ++;
  }
  return len;
  /*word_count_t * presListElem = list_entry(&wclist, word_count_t, elem);
  len = strlen(presListElem->word);
  return len;*/
  //word_count_t * wc = list_entry(wclist, word_count_t, elem);
  //return strlen(wc->word);
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  word_count_t * wcelem = NULL; //word count elem
  for (struct list_elem * e = list_begin(wclist); e != list_end(wclist); e = list_next(e)) {
    wcelem = list_entry(e, word_count_t, elem);
    if (strcmp(wcelem->word, word) == 0) {
      return wcelem;
    }
  }
  return wcelem;
}

/*word_count_t is the list node that stores word;
it embeds linked elem list_elem as "elem";
*/
word_count_t* add_word(word_count_list_t* wclist, char* word) {
  if (wclist == NULL) {
    return NULL;
  }
  word_count_t * wc = NULL; //element that embeds word count

  for (struct list_elem * e = list_begin(wclist); e != list_end(wclist); e = list_next(e)) {
    wc = list_entry(e, word_count_t, elem);
    if (strcmp(wc->word, word) == 0) {
      wc->count += 1;
      return wc;
    }
  }
  
  wc = (word_count_t * ) malloc(sizeof(word_count_t));
  wc->word = malloc(sizeof(char) * WORD_LEN);
  strcpy(wc->word, word);
  wc->count = 1;
  list_push_back(wclist, &(wc->elem));
  return wc;
  //return NULL;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) { /* TODO */
  word_count_t * wcelem;
  for (struct list_elem * e = list_begin(wclist); e != list_end(wclist); e = list_next(e)) {
    wcelem = list_entry(e, word_count_t, elem);
    fprintf(outfile, "%i\t%s\n", wcelem->count, wcelem->word);
  }
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  word_count_t* wc1 = list_entry(ewc1, word_count_t, elem);
  word_count_t* wc2 = list_entry(ewc2, word_count_t, elem);
  return strcmp(wc1->word, wc2->word) < 0;
}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  list_sort(wclist, less_list, less);
}
