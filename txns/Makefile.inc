OBJDIRS += txns

# Create template specific variables
UPPERC_DIR := TXNS
LOWERC_DIR := txns

$(UPPERC_DIR)_SRCS := txns/txn.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-txns: $(TXNS_TESTS)
	@for a in $(TXNS_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
