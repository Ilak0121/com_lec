ROOT:=../

include $(ROOT)/platform.mk

CFLAGS:=-c -mcpu=cortex-a9 -mfpu=neon-fp16
LDFLAGS:=

SOURCES:=matmul.c
HEADERS:=

OBJECTS:=$(SOURCES:.c=.o)

EXECUTABLE:=matmul

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

$(OBJECTS): $(HEADERS)


clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)

