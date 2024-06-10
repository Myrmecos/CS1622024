/*

Copyright © 2019 University of California, Berkeley

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

word_count provides lists of words and associated count

Functional methods take the head of a list as first arg.
Mutators take a reference to a list as first arg.
*/

#include "word_count.h"
#define STRING_LEN 64
/* Basic utilities */

char *new_string(char *str) {
  char *new_str = (char *) malloc(strlen(str) + 1);
  if (new_str == NULL) {
    return NULL;
  }
  return strcpy(new_str, str);
}

int init_words(WordCount **wclist) {
  /* Initialize word count.
     Returns 0 if no errors are encountered
     in the body of this function; 1 otherwise.
  */

  *wclist = NULL;
  return 0;
} //

ssize_t len_words(WordCount *wchead) {
  /* Return -1 if any errors are
     encountered in the body of
     this function.
  */
    if (wchead == NULL) {
      return -1;
    }
    size_t len = 0;
    len = strlen(wchead->word);
    return len;
} //

WordCount *find_word(WordCount *wchead, char *word) {
  /* Return count for word, if it exists */
  WordCount *wc = NULL;
  while (1) {
    if (strcmp(wchead->word, word) == 0) {
      wc = wchead;
      break;
    }
    wchead = wchead->next;
  }
  return wc;
}//

int add_word(WordCount **wclist, char *word) {
  /* If word is present in word_counts list, increment the count.
     Otherwise insert with count 1.
     Returns 0 if no errors are encountered in the body of this function; 1 otherwise.
  */
  if (*wclist == NULL) {
    (*wclist) = malloc(sizeof(WordCount));
    (*wclist)->word = malloc(sizeof(char) * STRING_LEN);
    strcpy((*wclist)->word, word);
    (*wclist)->count = 1;
    (*wclist)->next = NULL;
    return 0;
  } else {
    WordCount * ptr = *wclist;
    if (strcmp(ptr->word, word) == 0) {
      ptr->count += 1;
      return 0;
    }
    while (ptr->next != NULL) {
      if (strcmp(ptr->next->word, word)) {
        ptr->next->count += 1;
        return 0;
      }
      ptr = ptr->next;
    }
    ptr->next = malloc(sizeof(WordCount));
    ptr->next->word = malloc(sizeof(char) * STRING_LEN);
    strcpy(ptr->next->word, word);
    ptr->next->count = 1;
    return 0;
    
  }
  return 0;
} //

void fprint_words(WordCount *wchead, FILE *ofile) {
  /* print word counts to a file */
  WordCount *wc;
  //printf("The first word is: %s\n", wchead->word);
  for (wc = wchead; wc; wc = wc->next) {
    //printf("%s\t%d", wc->word, wc->count);
    fprintf(ofile, "%i\t%s\n", wc->count, wc->word);
  }
}
