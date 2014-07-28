#################################################
#
# $@:   目标文件
# $^:   所有的依赖文件
# $<:   第一个依赖文件.
#
#################################################
include ./rule.in

# {exe(default), static, shared}
TARGET := rmnt
BINARY_TYPE := exe

ifndef PUB_DIR
export PUB_DIR := $(HISI_SRC)/../pub
endif

CFG_INC = -I$(PUB_DIR)/include 


LIBPATH = -L$(PUB_DIR)/lib
LIBPATH += -L./lib

LIB = -Wl,--start-group -lpthread -lrt -ldl -lm -lz -Wl,--end-group

LIB += 

DEFS := 
LINK_LIBS := 
INCDIR += -I./include
INCDIR += -I../
LFLAGS := 

.PHONY: all
all: createdir $(TARGET)
	chmod a+x $(TARGET)
	@echo "-------- $(TARGET) done ---------"

SOURCES_C := $(wildcard *.c)
OBJS_C := $(patsubst %.c, %.o, $(SOURCES_C))
OBJS := $(OBJS_C) 

OBJDIR := .objs


MKDIR := mkdir -p
RM := rm -rf

.PHONY: createdir
createdir: 
	$(MKDIR) $(OBJDIR)

%.o: %.c
	$(CC)  $(DEFS) $(INCDIR) $(CFG_INC) -c $< -o $(OBJDIR)/$@ 

#%.o: %.cpp
#	$(CXX)  $(DEFS) $(INCDIR) $(CFG_INC) -c $< -o $(OBJDIR)/$@ 

$(TARGET): $(OBJS)
ifeq ($(BINARY_TYPE), static)
	$(AR) rc $@ $(addprefix $(OBJDIR)/, $^)
else
ifeq ($(BINARY_TYPE), shared)
	$(CXX) -shared -fPIC -o $@ $(addprefix $(OBJDIR)/, $^) $(CFG_INC) $(LIBPATH) $(LIB)
else
	$(CXX)  -o $@ $(addprefix $(OBJDIR)/, $^)  $(CFG_INC) -lstdc++  $(LIBPATH) $(LIB)
 endif
endif

.PHONY: clean
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJDIR)

