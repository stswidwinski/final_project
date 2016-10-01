OBJDIRS += containers

# Create template specific variables
UPPERC_DIR := CONTAINERS
LOWERC_DIR := containers

CONTAINERS_SRCS := containers/container.cc containers/array_cont.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/txns/txn.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-containers: $(CONTAINERS_TESTS)
	@for a in $(CONTAINERS_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
