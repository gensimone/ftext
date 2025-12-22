/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void die(const char *fmt, ...)
{
	va_list ap;
	int saved_errno;

	saved_errno = errno;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':')
		fprintf(stderr, " %s", strerror(saved_errno));
	fputc('\n', stderr);

	exit(1);
}

char* next_word()
{
        char* word = (char*) malloc(sizeof(char) * 100);
        int c;
        int in_word = 0;
        for (int i = 0; (c = getchar()) != EOF; i++) {
                if (c == ' ' || c == '\n' || c == '\t' && in_word) break;
                else {
                        word[i] = c;
                        in_word = 1;
                }
        }
        return word;
}

char* strspace(size_t n)
{
        char* dst;
        if (!(dst = (char*) malloc(n + 1)))
                die("cannot malloc %zu bytes:", n + 1);
        memset(dst, ' ', n);
        dst[n] = '\0';
        return dst;
}

char* align_strings(char* dst, int n)
{
        int t = 0;
        int string_count = 0;
        char** words_list;
        if (!(words_list = (char**) malloc(sizeof(char*) * 10)))
                die("cannot malloc %zu bytes:", sizeof(char*) * 10);
        for (int i = 0; t < n; i++) {
                char* word = next_word();
                int length = strlen(word);
                if (length == 0)
                        break;
                if (string_count > 0 && (t + length + string_count) > n) {
                        strcpy(dst, word);
                        break;
                }
                words_list[i] = word;
                string_count++;
                t += strlen(word);
        }

        if (string_count == 1) {
                if (t > n) {
                        char* substring;
                        if (!(substring = (char*) malloc(n + 1)))
                                die("cannot malloc %zu bytes:", n);
                        strncpy(substring, words_list[0], n);
                        strcpy(dst, words_list[0] + n);
                        return substring;
                }
                return words_list[0];
        }

        // do not consider last word.
        // we already loaded that word in dst.
        if (t > n) string_count -= 1;

        char* str_output;
        if (!(str_output = (char*) malloc(n + 1)))
                die("cannot malloc %zu bytes:", n + 1);

        strcpy(str_output, words_list[0]);

        int n_internal_words = string_count - 2;
        int n_tot_spaces = n - t;

        for (int i = 1; i < string_count; i++) {
                double res = (double) n_tot_spaces / (n_internal_words + 1);
                int n_spaces = ceil(res);
                n_tot_spaces -= n_spaces;
                n_internal_words -= 1;
                char* space = strspace(n_spaces);

                strcat(str_output, space);
                strcat(str_output, words_list[i]);
                free(words_list[i]);
        }
        free(words_list);

        return str_output;
}
