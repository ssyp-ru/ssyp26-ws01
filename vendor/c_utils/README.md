# C utilities

Небольшой набор утилит, используемых для работы со студентами.

## Console colors


```c
printf(COLOR_GREEN "Success" COLOR_RESET "\n");
printf(COLOR_BRIGHT_RED "Error: %s" COLOR_RESET "\n", message);
```

## Logging

```c
#include "utils.h"

int main(void)
{
    log_set_level(LOG_LEVEL_DEBUG);
    log_set_level_string("debug");

    log_debug("loaded %d records", 10);
    log_info("application started");
    log_error("could not open %s", "config.txt");
    return 0;
}
```

Настройка записи в файл:
```c
if (log_set_file("application.log") != 0) {
    log_error("could not open the log file");
}

log_close_file();
```
