#include "read_files.h"

int rf_load_next_file(rf) {
  close(0);					
  int reti=0;
  while ( !(rf->flags.valid) && rf->argc > 0 ) {
    if ((reti = open(rf->argv, O_RDONLY, 0)) < 0) {
      perror("open");
    } else {
      rf->flags.valid = TRUE;
    }
    rf->argc -= 1;
    rf->argv += 1;
  }
}

int rf_readline(rf_data *rf, char *line, size_t *len) {
  if (rf->flags.valid) {
    if (rf->mmap) {
    } else {
      reti = getline(&line, &len, stdin);
      if (reti == -1)
	{
	  rf->flags.valid = FALSE;
	  return reti;
	}
    }
  } else {
    if (rc.argc > 0) {
      rf_load_next_file(rf);
      return rf_readline(rf, &line, &lin);
    } else {
      return -1;
    }
  }
}
 
} else {
    if (rf.argc > 0) {
      while(rf.argc > 0) {
	close(0);
	if (open(*argv, O_RDONLY, 0) < 0)
	  perror("Couldn't open file");
	else {
	  mgrep(&preg, flags);
	}
	rf.argc -= 1;
	rf.argv += 1;
      }
    }
    if (rf.flags.stdin) {
    }
  }
}
