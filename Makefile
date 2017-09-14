CC ?= clang
CE_DIR ?= ce
CFLAGS := -Wall -Werror -Wshadow -Wextra -Wno-unused-parameter -std=gnu11 -ggdb3 -fPIC
LDFLAGS := -rdynamic -shared -lncursesw

.PHONY: all clean

all: ce_config.so

ce_config.so: ce_config.c $(CE_DIR)/ce_app.c $(CE_DIR)/ce.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f ce_config.so
