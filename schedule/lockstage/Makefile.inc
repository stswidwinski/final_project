OBJDIRS += schedule/lockstage

# Create template specific variables
UPPERC_DIR := SCHEDULE/LOCKSTAGE
LOWERC_DIR := schedule/lockstage

$(UPPERC_DIR)_SRCS := schedule/lockstage/lockstage.cc  

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/txns/txn.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-schedule/lockstage: $(SCHEDULE/LOCKSTAGE_TESTS)
	@for a in $(SCHEDULE/LOCKSTAGE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
