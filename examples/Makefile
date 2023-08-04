CC := gcc
CPPFLAGS := -I../include
CFLAGS := -Wall -Wextra -pedantic -std=c17 -Og -g
LDFLAGS := -mconsole -static -L../bin/debug
LDLIBS := -lpyrex

src_dir := .
examples_dir := examples
build_dir := ../build/$(examples_dir)
bin_dir := ../bin/$(examples_dir)

src := $(wildcard $(src_dir)/*.c)
obj = $(patsubst $(src_dir)/%.c,$(build_dir)/%.o,$(src))
examples := $(patsubst $(build_dir)/%.o,$(bin_dir)/%,$(obj))

.PHONY: all clean remake info
.SECONDARY: $(obj)

all: $(examples)

$(bin_dir)/%: $(build_dir)/%.o
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

$(build_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(obj)
	$(RM) $(examples)

remake: clean all

info:
	@echo $(CC)
	@echo $(CPPFLAGS)
	@echo $(CFLAGS)
	@echo $(LDFLAGS)
	@echo $(LDLIBS)
	@echo $(src_dir)
	@echo $(examples_dir)
	@echo $(build_dir)
	@echo $(bin_dir)
	@echo $(src)
	@echo $(obj)
	@echo $(examples)

