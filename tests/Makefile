CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -I.. -I../inc -I../nanopb -I../whad/protocol -I../whad/protocol/board

TEST_BINS := test_transport test_board test_ble test_esb test_phy

# Shared source list: every domain test textually includes its domain .c file
# (validators are static, invisible to a separate TU). whad.c provides
# whad_get_message_domain, which transitively requires transport.c, ringbuf.c,
# and ALL domain .pb.c files (nanopb message descriptors for the Message oneof).
NANOPB_SRCS := ../nanopb/pb_common.c ../nanopb/pb_decode.c ../nanopb/pb_encode.c
PROTOCOL_SRCS := $(wildcard ../whad/protocol/*.c) $(wildcard ../whad/protocol/*/*.c)
DOMAIN_TEST_COMMON_SRCS := ../src/whad.c ../src/transport.c ../src/ringbuf.c $(PROTOCOL_SRCS) $(NANOPB_SRCS)

# test_transport: standalone TU, includes transport.h only
test_transport_SRCS := test_transport.c ../src/transport.c ../src/ringbuf.c

# test_board: board.c textually included by test_board.c. phy.c is linked
# separately to provide whad_phy_message_free (called by whad.c cleanup).
test_board_SRCS := test_board.c ../src/domains/phy.c $(DOMAIN_TEST_COMMON_SRCS)

# test_ble: ble.c textually included by test_ble.c. phy.c linked separately
# (same reason as test_board — whad_phy_message_free is invoked by whad.c).
test_ble_SRCS := test_ble.c ../src/domains/phy.c $(DOMAIN_TEST_COMMON_SRCS)

# test_esb: esb.c textually included by test_esb.c. phy.c linked separately.
test_esb_SRCS := test_esb.c ../src/domains/phy.c $(DOMAIN_TEST_COMMON_SRCS)

# test_phy: phy.c textually included by test_phy.c. phy.c MUST NOT also be
# linked separately (duplicate-symbol error). whad_phy_message_free is
# provided by the textual inclusion.
test_phy_SRCS := test_phy.c $(DOMAIN_TEST_COMMON_SRCS)

.PHONY: test clean

test: $(TEST_BINS)
	@for bin in $(TEST_BINS); do echo "Running $$bin..."; ./$$bin || exit 1; done

test_transport: $(test_transport_SRCS)
	$(CC) $(CFLAGS) $(test_transport_SRCS) -o $@

test_board: $(test_board_SRCS)
	$(CC) $(CFLAGS) $(test_board_SRCS) -o $@

test_ble: $(test_ble_SRCS)
	$(CC) $(CFLAGS) $(test_ble_SRCS) -o $@

test_esb: $(test_esb_SRCS)
	$(CC) $(CFLAGS) $(test_esb_SRCS) -o $@

test_phy: $(test_phy_SRCS)
	$(CC) $(CFLAGS) $(test_phy_SRCS) -o $@

clean:
	rm -f $(TEST_BINS)
