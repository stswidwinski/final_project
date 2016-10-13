OBJDIRS += packing

# Create template specific variables
UPPERC_DIR := PACKING
LOWERC_DIR := packing

$(UPPERC_DIR)_SRCS := packing/packing.cc packing/packing_test.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/txns/txn.o obj/containers/array_cont.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-packing: $(PACKING_TESTS)
	@for a in $(PACKING_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
