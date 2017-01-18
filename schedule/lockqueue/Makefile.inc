OBJDIRS += schedule/lockqueue

# Create template specific variables
UPPERC_DIR := SCHEDULE/LOCKQUEUE
LOWERC_DIR := schedule/lockqueue

$(UPPERC_DIR)_SRCS := schedule/lockqueue/lockqueue.cc  

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/schedule/lockstage/lockstage.o obj/txns/txn.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-schedule/lockqueue: $(SCHEDULE/LOCKQUEUE_TESTS)
	@for a in $(SCHEDULE/LOCKQUEUE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
