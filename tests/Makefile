CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -I.. -I../inc -I../nanopb -I../whad/protocol -I../whad/protocol/board

TEST_BINS := test_transport test_board

# test_transport: standalone TU, includes transport.h only
test_transport_SRCS := test_transport.c ../src/transport.c ../src/ringbuf.c

# test_board: board.c is textually included by test_board.c (validators are
# static and invisible to a separate TU). whad.c provides whad_get_message_domain,
# which transitively requires transport.c, ringbuf.c, and ALL domain .pb.c files
# (nanopb message descriptors for the Message oneof). phy.c is linked to
# provide whad_phy_message_free (called by whad.c during message cleanup).
test_board_SRCS := test_board.c \
	../src/whad.c ../src/transport.c ../src/ringbuf.c ../src/domains/phy.c \
	$(wildcard ../whad/protocol/*.c) $(wildcard ../whad/protocol/*/*.c) \
	../nanopb/pb_common.c ../nanopb/pb_decode.c ../nanopb/pb_encode.c

.PHONY: test clean

test: $(TEST_BINS)
	@for bin in $(TEST_BINS); do echo "Running $$bin..."; ./$$bin || exit 1; done

test_transport: $(test_transport_SRCS)
	$(CC) $(CFLAGS) $(test_transport_SRCS) -o $@

test_board: $(test_board_SRCS)
	$(CC) $(CFLAGS) $(test_board_SRCS) -o $@

clean:
	rm -f $(TEST_BINS)
