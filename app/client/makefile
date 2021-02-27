CC = gcc

CFLAGS = -ansi -pedantic -Wall -Werror -g3

# c pre processor
CFLAGS += -I$(INC_DIR)
CFLAGS += -I$(INC_DS_DIR)
INC_DIR = ../../inc
INC_DS_DIR = ../../inc/data_structures
SRC_DIR = ../../src
SRC_MULTICAST_DIR = ../../src/multicast
# add shared sources that we depend on
SOURCES_NEEDED = client_app.c client_net.c protocol.c ui_manager.c
MULTICAST_SOURCES_NEEDED = multicast.c

SOURCE_TEST = app_client.c
TARGET = app_client

SOURCES = $(foreach src, $(SOURCES_NEEDED), $(SRC_DIR)/$(src))
SOURCES += $(foreach src, $(MULTICAST_SOURCES_NEEDED), $(SRC_MULTICAST_DIR)/$(src))
SOURCES += $(SOURCE_TEST)

OBJECTS := $(SOURCES:.c=.o)
all : $(TARGET)
$(TARGET):   $(OBJECTS)
	$(CC) $(OBJECTS) -L../../lib -lHashMap -lQueue -o $(TARGET)
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
