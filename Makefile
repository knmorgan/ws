CC = g++
OBJDIR = obj
TARGET = ws
CXXFILES = $(wildcard src/*.cc)
OBJFILES = $(patsubst src/%.cc,$(OBJDIR)/%.o,$(CXXFILES))
CXXFLAGS = -Wall -Werror -pedantic -O2
LDFLAGS = -lgmpxx -lgmp
RM = rm -rf

all: $(TARGET)

ws: $(OBJFILES)
	$(CC) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.cc src/%.h
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(OBJFILES): | $(OBJDIR)

$(OBJDIR):
	test -d $(OBJDIR) || mkdir $(OBJDIR)

clean:
	$(RM) $(OBJDIR) $(TARGET)
