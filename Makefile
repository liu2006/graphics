CC = clang
CFLAGS = -Wall -Wextra -Iinclude -Ivendor/glfw/include -Ivendor/vulkan/x86_64/include 
CPPFLAGS = -Iinclude
LDFLAGS = -Lvendor/vulkan/x86_64/lib -Lvendor/glfw/lib
LDLIBS = -lvulkan -lglfw3 -lm

TARGET = build/render
TARGETDIR = build
OBJDIR = $(TARGETDIR)/obj
SRCDIR = src

OBJS := $(addprefix $(OBJDIR)/, commandBuffer.o device.o dynamic.o indexBuffer.o instance.o main.o\
	physicalDevice.o pipeline.o recordCommand.o swapChain.o uniformBuffers.o vertexBuffer.o)
SRCS := $(addprefix $(SRCDIR)/, commandBuffer.c device.c dynamic.c indexBuffer.c instance.c main.c\
	physicalDevice.c pipeline.c recordCommand.c swapChain.c uniformBuffers.c vertexBuffer.c)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

.PHONY: all clean install  
all: $(TARGET)

$(TARGET): $(OBJS) | $(TARGETDIR)
	clang -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(TARGETDIR):
	mkdir -p $@ 

$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(TARGETDIR)


