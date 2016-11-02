OBJDIRS += simulation

# Create template specific variables
UPPERC_DIR := SIMULATION
LOWERC_DIR := simulation

$(UPPERC_DIR)_SRCS := simulation/txn_generator.cc simulation/txn_map.cc simulation/sim_args_parser.cc simulation/simulation_test.cc 

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/txns/txn.o obj/schedule/schedule.o obj/schedule/batchschedule.o obj/schedule/locktable.o obj/schedule/lockqueue.o obj/schedule/lockstage.o obj/containers/array_cont.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-simulation: $(SIMULATION_TESTS)
	@for a in $(SIMULATION_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
 
