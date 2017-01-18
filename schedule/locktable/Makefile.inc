OBJDIRS += schedule/locktable

# Create template specific variables
UPPERC_DIR := SCHEDULE/LOCKTABLE
LOWERC_DIR := schedule/locktable

$(UPPERC_DIR)_SRCS := schedule/locktable/locktable.cc schedule/locktable/readytxnqueue.cc 

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/schedule/lockqueue/lockqueue.o obj/schedule/lockstage/lockstage.o obj/txns/txn.o obj/schedule/locktable/readytxnqueue.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-schedule/locktable: $(SCHEDULE/LOCKTABLE_TESTS)
	@for a in $(SCHEDULE/LOCKTABLE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
