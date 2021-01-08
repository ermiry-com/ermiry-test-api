TARGET      := test-api

PTHREAD 	:= -l pthread
MATH 		:= -lm

OPENSSL		:= -l ssl -l crypto

# CMONGO 		:= `pkg-config --libs --cflags libmongoc-1.0`
MONGOC 		:= -l mongoc-1.0 -l bson-1.0
MONGOC_INC	:= -I /usr/local/include/libbson-1.0 -I /usr/local/include/libmongoc-1.0

CMONGO		:= -l cmongo
CMONGO_INC	:= -I /usr/local/include/cmongo

CERVER		:= -l cerver
CERVER_INC	:= -I /usr/local/include/cerver

DEVELOPMENT	:= -g -D ERMIRY_DEBUG

CC          := gcc

SRCDIR      := src
INCDIR      := include
BUILDDIR    := objs
TARGETDIR   := bin

SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

CFLAGS      := $(DEVELOPMENT) -Wall -Wno-unknown-pragmas
LIB         := -L /usr/local/lib $(PTHREAD) $(MATH) $(OPENSSL) $(MONGOC) $(CERVER) $(CMONGO)
INC         := -I $(INCDIR) -I /usr/local/include $(MONGOC_INC) $(CERVER_INC) $(CMONGO_INC)
INCDEP      := -I $(INCDIR)

SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: directories $(TARGET)

run:
	./$(TARGETDIR)/$(TARGET)

directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

clean:
	@$(RM) -rf $(BUILDDIR) 
	@$(RM) -rf $(TARGETDIR)

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# link
$(TARGET): $(OBJECTS)
	$(CC) $^ $(LIB) -o $(TARGETDIR)/$(TARGET)

# compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

.PHONY: all clean