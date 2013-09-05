typedef struct {
    int fp;
    flags_t flags;
} rf_data;

int rf_readline(rf_data *rf, char *line, size_t *len);
