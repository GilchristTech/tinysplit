/*
    TinySplit Example: C With Memory Management

    This example shows a TinySplit function which handles memory
    allocation of strings in C. As such, it's longer than many
    other TinySplit implementations in languages which handle
    memory allocation themselves, or even a C solution which
    liberally allocates and frees memory.

    It works by organizing the stack and string data with each
    iteration in a state struct, and copying trimmed strings into
    the same memory allocation as the strings on the stack, which
    allows all string memory (other than the input line) to be
    resized and freed in a minimum number of calls.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


typedef struct {
    size_t * stack;  // Stack of memory offsets from the start of
                     // the string buffer (string_pool)
    size_t   stack_size;
    size_t   stack_length;

    char   * string_pool;
    size_t   string_pool_size;
    size_t   string_pool_length;

    char   * push;
    char  ** pop;

    char   * line;
    char   * trimmed;
    char     sigil;

    size_t   trimmed_length;
    size_t   line_length;
} TinysplitState;


void tinysplitStateLineByLength (TinysplitState * s, char * line, int length) {
    if (s->stack == NULL) {
        s->stack_length = 0;
        s->stack_size   = sizeof(size_t *) * 8;
        s->stack        = malloc(s->stack_size);

        s->string_pool_length = 0;
        s->string_pool_size   = sizeof(char) * 32;
        s->string_pool        = malloc(s->string_pool_size);
    }

    s->push = NULL;
    s->pop  = NULL;

    if (line == NULL) {
        goto RETURN_NONE;
    }

    s->line = line;
    s->line_length = length;

    // Calculate the start and end pointers of the line
    char *end;
    for (end=&line[length-1]; end >= line; end--) {
        if (!isspace(*end)) break;
    }

    char *start;
    for (start=line; start < end; start++) {
        if (!isspace(*start)) break;
    }

    // Copy the string content within the trimmed bounds of the
    // input line into the stack's string buffer. If the value is
    // pushed onto the stack, there is a string reference; and if
    // not, there is a trimmed line reference with a lifetime
    // lasting until the next run of this function.

    s->trimmed_length = end - start + 1;

    while (s->string_pool_length + s->trimmed_length >= s->string_pool_size) {
        s->string_pool = realloc(s->string_pool, s->string_pool_size *= 2);
    }

    s->trimmed = &s->string_pool[s->string_pool_length];

    char *dest = &s->string_pool[s->string_pool_length];

    if (s->trimmed_length > 0) {
        memcpy(dest, start, s->trimmed_length);
    }
    dest += s->trimmed_length;
    *dest++ = '\x0';

    s->sigil = *s->trimmed;

    int pop_from = -1;

    switch (s->sigil) {
        case '(':
        case ':':
            s->push = s->trimmed;
            break;

        case '@':
            for (int p=s->stack_length-1; p >= 0; p--) {
                switch (s->string_pool[s->stack[p]]) {
                    default:  continue;
                    case '@': pop_from = p;
                    case '(': break;
                }
                break;
            }

            if (s->trimmed_length > 1) {
                s->push = s->trimmed;
            }
            break;

        case ')':
            for (pop_from = s->stack_length-1; pop_from > 0; pop_from--) {
                if (s->string_pool[s->stack[pop_from]] == '(')
                    break;
            }
            break;
    }

    if (pop_from >= 0) {
        s->stack_length = pop_from;
    }

    if (s->push) {
        while ((s->stack_length+1) * sizeof(*s->stack) >= s->stack_size) {
            s->stack = realloc(s->stack, s->stack_size *= 2);
        }

        // Because the trimmed line has already been copied into
        // stack string buffer memory, all that is needed to add
        // it to the stack is reference the start of the trimmed
        // line.
        //
        s->stack[s->stack_length++] = s->string_pool_length;
        s->string_pool_length = dest - s->string_pool;
    }

    return;

RETURN_NONE:
    s->sigil   = '\0';
    s->trimmed = NULL;
    s->line    = NULL;
}


void freeTinysplitStateProperties (TinysplitState *s) {
    free(s->stack);
    free(s->string_pool);
    s->stack     = NULL;
    s->string_pool = NULL;
}


void freeTinysplitState (TinysplitState *s) {
    freeTinysplitStateProperties(s);
    free(s);
}


char * tinysplitStateGetStack (TinysplitState *s, int n) {
    if (n < 0)
        n += s->stack_length-1;
    return &s->string_pool[s->stack[s->stack_length - 1 - n]];
}


int main (int argc, char ** argv) {
    FILE *input;
    
    if (argc == 1) {
        input = stdin;
    } else if (argc != 2) {
        fprintf(stderr, "%s expects 1 or 2 arguments, got %d\n", argv[0], argc-1);
        return 1;
    } else {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("Error opening file");
            return 1;
        }
    }

    char * line = NULL;
    size_t size = 0;
    int  length = 0;

    TinysplitState state = {};

    while ((length = getline(&line, &size, input)) != -1) {
        if (line[length-1] == '\n') {
            line[--length] = '\0';
        }

        tinysplitStateLineByLength(&state, line, length);
        printf("Trimmed: \"%s\"\n", state.trimmed     );
        printf("Sigil: %c\n",       state.sigil       );
        printf("stack[%d]:\n",      (int) state.stack_length);

        for (int i=0; i < state.stack_length; i++) {
            printf("  - %s\n", tinysplitStateGetStack(&state, i));
        }
        printf("\n");
    }

    free(line);
    freeTinysplitStateProperties(&state);
    fclose(input);

    return 0;
}
