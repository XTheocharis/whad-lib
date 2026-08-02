CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -I.. -I../inc -I../nanopb -I../whad/protocol -I../whad/protocol/board

TEST_BIN := test_transport
TEST_SRCS := test_transport.c ../src/transport.c ../src/ringbuf.c

.PHONY: test clean

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_SRCS) -o $@

clean:
	rm -f $(TEST_BIN)
