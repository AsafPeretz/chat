CC = gcc

CFLAGS = -ansi -pedantic -Wall -Werror -g3

# c pre processor
CFLAGS += -I$(INC_DIR)
INC_DIR = ../../inc
SRC_DIR = ../../src/multicast
# add shared sources that we depend on
SOURCES_NEEDED = multicast.c

TARGET = test_multicast

SOURCES = $(foreach src, $(SOURCES_NEEDED), $(SRC_DIR)/$(src))

OBJECTS := $(SOURCES:.c=.o)
all : $(TARGET)
$(TARGET):   $(OBJECTS)
run:  $(TARGET)
	 ./$(TARGET)
check: clean run
grind: valgrind helgrind
valgrind:  $(TARGET)
	 valgrind --log-file=valgrind.log --leak-check=full --track-origins=yes ./$(TARGET) 
helgrind:  $(TARGET)
	 valgrind --tool=helgrind --log-file=helgrind.log ./$(TARGET) 
clean:
	 $(RM) $(TARGET) $(SRC_DIR)/*.o $(SRC_DIR)/*.d $(SRC_DIR_TEST)/*.o $(SRC_DIR_TEST)/*.d *.o *.d
.PHONY: all clean run check grind valgrind helgrind 
#Collapse

DEPEND = $(OBJECTS:.o=.d)

-include $(DEPEND)
