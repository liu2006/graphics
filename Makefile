CC = clang
CFLAGS = -Wall -Wextra -Iinclude -Ivendor/glfw/include -Ivendor/vulkan/x86_64/include
CPPFLAGS = -Iinclude
LDFLAGS = -Lvendor/vulkan/x86_64/lib -Lvendor/glfw/lib
LDLIBS = -lvulkan -lglfw3 -lm

TARGET = build/render
TARGETDIR = build

OBJDIR := $(TARGETDIR)/obj
OBJS := $(addprefix $(OBJDIR)/, commandBuffer.o device.o dynamic.o indexBuffer.o instance.o main.o\
	physicalDevice.o pipeline.o recordCommand.o swapChain.o uniformBuffers.o vertexBuffer.o)
SRCDIR := src
SRCS := $(addprefix $(SRCDIR)/, commandBuffer.c device.c dynamic.c indexBuffer.c instance.c main.c\
	physicalDevice.c pipeline.c recordCommand.c swapChain.c uniformBuffers.c vertexBuffer.c)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS) | $(TARGETDIR)
	clang -o $@ $(LDFLAGS) $^ $(LDLIBS)	
$(TARGETDIR):
	mkdir $(TARGETDIR) 

$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: clean

clean:
	rm $(TARGET) $(OBJS)
