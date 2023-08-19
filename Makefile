CC := gcc

CPPFLAGS :=
release_cppflags := -DNDEBUG
debug_cppflags := -DDEBUG

CFLAGS := -Wall -Wextra -pedantic -std=c17
release_cflags := -O2
debug_cflags := -O0 -ggdb3

LDFLAGS := -static
LDLIBS :=

lib_src_dir := src
examples_src_dir := examples
build_dir := build
bin_dir := bin
lib_dir := lib
release_dir := release
debug_dir := debug

src := $(wildcard $(lib_src_dir)/*.c)
release_obj = $(patsubst $(lib_src_dir)/%.c,$(build_dir)/$(release_dir)/%.o,$(src))
debug_obj = $(patsubst $(lib_src_dir)/%.c,$(build_dir)/$(debug_dir)/%.o,$(src))

lib := pyrex
pyrex_release := $(lib_dir)/$(release_dir)/lib$(lib).a
pyrex_debug := $(lib_dir)/$(debug_dir)/lib$(lib).a

.PHONY: all \
		pyrex.release \
		pyrex.debug \
		examples \
		clean.release \
		clean.debug \
		clean.examples \
		clean \
		info

all: pyrex.debug examples

pyrex.release: $(pyrex_release)
	@echo $(done_building)

pyrex.debug: $(pyrex_debug)
	@echo $(done_building)

$(pyrex_release): $(release_obj)
	@echo $(linking)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

$(pyrex_debug): $(debug_obj)
	@echo $(linking)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

$(build_dir)/$(release_dir)/%.o: $(lib_src_dir)/%.c
	@echo $(compiling)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(release_cppflags) $(CFLAGS) $(release_cflags) -c $< -o $@

$(build_dir)/$(debug_dir)/%.o: $(lib_src_dir)/%.c
	@echo $(compiling)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(debug_cppflags) $(CFLAGS) $(debug_cflags) -c $< -o $@


examples: $(pyrex_debug)
	$(MAKE) -C $(examples_src_dir)
	@echo $(done_building)


clean.release:
	@echo $(cleaning)
	$(RM) $(release_obj)
	$(RM) $(pyrex_release)

clean.debug:
	@echo $(cleaning)
	$(RM) $(debug_obj)
	$(RM) $(pyrex_debug)

clean.examples:
	@echo $(cleaning)
	$(MAKE) -C $(examples_src_dir) clean

clean: clean.release clean.debug clean.examples


info:
	@echo $(info)
	@echo $(CC)
	@echo $(CPPFLAGS)
	@echo $(CFLAGS)
	@echo $(LDFLAGS)
	@echo $(LDLIBS)
	@echo $(lib_src_dir)
	@echo $(build_dir)
	@echo $(bin_dir)
	@echo $(release_dir)
	@echo $(debug_dir)
	@echo $(src)
	@echo $(release_obj)
	@echo $(debug_obj)
	@echo $(lib)
	@echo $(pyrex_release)
	@echo $(pyrex_debug)
	@echo $(examples_src_dir)

done_building = -e "$(green)$(invert) Done building $(reset) $(green)$@$(reset)"
linking = -e "$(cyan)$(invert) Linking $(reset) $(cyan)$^$(reset)"
compiling = -e "$(cyan)$(invert) Compiling $(reset) $(cyan)$<$(reset)"
running = -e "$(magenta)$(invert) Running $(reset) $(magenta)$<$(reset)"
cleaning = -e "$(red)$(invert) Cleaning $(reset)"
info = -e "$(cyan)$(invert) Info $(reset)"

# ANSI escape sequences
reset := \e[m
invert := \e[7m
red := \e[91m
green := \e[92m
blue := \e[94m
magenta := \e[95m
cyan := \e[96m

