#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <regex.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

typedef unsigned int boolean_t;
#define FALSE 0
#define TRUE !FALSE
typedef union {
    struct {
	boolean_t mmap:1;
	int :30;                /* unused */
	boolean_t compat:1;     /* bit 31 */
    };
    int raw;
} flags_t;

void readlines(const regex_t *preg, char* str, const size_t size);
void read_stdin(const regex_t* preg);
void mgrep(const regex_t* preg, flags_t flags);

void read_stdin(const regex_t* preg) {
    ssize_t read;
    char *line = NULL;
    size_t len = 0;

    while ((read = getline(&line, &len, stdin)) != -1) {
	if (regexec(preg, line, 0, NULL, 0) ==0) {
	    printf("%s", line);
	}
    }
}

void mgrep(const regex_t* preg, flags_t flags) {
    if (flags.mmap) {
	struct stat sb;
	if (fstat(0, &sb) == -1) {
	    perror ("fstat");
	    exit(1);
	}
	char *p =mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, 0, 0);
	if (p == MAP_FAILED) {
	    perror("mmap");
	    exit(1);
	}
	readlines(preg, p, sb.st_size);
	munmap(p,sb.st_size);
    } else {
	read_stdin(preg);
    }
}

void readlines(const regex_t *preg, char* str, const size_t size) {
    fprintf(stderr, "Using mmaped file of size %zu\n", size);
    size_t buffsize=1024;
    char *line = (char *)malloc(buffsize*sizeof(char));
    int j;
    size_t bytes_read = 0;
    size_t lines=0;
    while(bytes_read < size) {
	char *nl = strchr(str, '\n');
	if (nl == NULL || nl < str) {
	    //TODO: if we didn't find a newline in the string should we return a pointer to end of string?
	    // or assume all reasonable text files will have a terminating new line character?
	    break;
	}
	lines += 1;
	size_t lsize = nl-str;
	if (bytes_read + lsize <= size) {      
	    if (lsize+1 >= buffsize) { //resize line buffer as needed
		buffsize *= 2;
		line = realloc(line, buffsize*sizeof(char));
	    }
	    memcpy(line, str, lsize+1);
	    line[lsize+1] = '\0';
	    if (regexec(preg, line, 0, NULL, 0) ==0) {
		printf("%s", line);
	    }
	    bytes_read += nl-str;
	    str += lsize+1;
	} else {
	    fprintf(stderr, "reading the next line would take us past the end of the mmap'ed space\n");
	    break;
	}
    }
    free(line);
    fprintf(stderr, "Processed %zu lines\n", lines);
}

int main(int argc, char *argv[]) {
    // get options
    int opt;
    flags_t flags;
    flags.mmap = FALSE;
    while ((opt = getopt(argc, argv, "m")) != -1) {
	switch(opt) {
	case 'm':
	    flags.mmap = 1;
	    break;
	default:
	    fprintf(stderr, "Usage: %s regex [[FILE1] [FILE2] ... [FILEN]]\n", argv[0]);
	    exit(1);
	}
    }
    argc -= optind;
    argv += optind;

    // consume regex
    regex_t preg;
    char buff[1024];
    int reti;
    if ((reti = regcomp(&preg,*argv, REG_EXTENDED)) != 0) {
	regerror(reti, &preg, buff, sizeof(buff));
	fprintf(stderr, "Regex error: %s\n", buff);
	exit(1);
    }
    argc -= 1;
    argv += 1;

    if (argc > 0) {
	while(argc > 0) {
	    close(0);
	    if (open(*argv, O_RDONLY, 0) < 0)
		perror("Couldn't open file");
	    else {
		mgrep(&preg, flags);
	    }
	    argc -= 1;
	    argv += 1;
	}
    } else {
	mgrep(&preg, flags);
    }
    regfree(&preg);
}
