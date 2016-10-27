OBJDIRS += simulation

# Create template specific variables
UPPERC_DIR := SIMULATION
LOWERC_DIR := simulation

$(UPPERC_DIR)_SRCS := simulation/txn_generator.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/txns/txn.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-simulation: $(SIMULATION_TESTS)
	@for a in $(SIMULATION_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
