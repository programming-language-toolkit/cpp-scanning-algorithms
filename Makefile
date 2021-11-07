# Practical makefiles, by example: http://nuclear.mutantstargoat.com/articles/make/
# Automatic variables: https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html

CXX      := g++
CXXFLAGS := -std=c++20 -fmodules-ts -fmax-errors=10
CPPFLAGS := -MMD

INCLUDES := -Iexternal -Isrc

# Set the display of the time command. See https://man7.org/linux/man-pages/man1/time.1.html
TIME_FORMAT := "%E elapsed  (%U user  %S system)  |  %P CPU  |  %Xk text  %Dk data  %Mk max  |  %I inputs  %O outputs  |  %F major + %R minor pagefaults  |  %W swaps\n"

COMPILE = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(BUILD_FLAGS) $(INCLUDES)
LINK    = $(CXX) $(INCLUDES)


.PHONY: all
all:
	$(MAKE) tests
	$(MAKE) docs


# ======================================================================================================================
# Functions
# ======================================================================================================================
define compile
	@echo "building $(@F) ..."
	@mkdir -p $(@D)
	@time -f $(TIME_FORMAT) -- $(COMPILE) -o $@ -c $<
endef


define link
	@echo "building $(@F) ..."
	@time -f $(TIME_FORMAT) -- $(LINK) -o $@ $^
endef


# ======================================================================================================================
# Tests
# ======================================================================================================================
TEST_SRCS := $(filter-out tests/main.test.cpp,$(shell find tests/ -name "*.test.cpp"))
TEST_EXES := $(addprefix build/,$(TEST_SRCS:.cpp=.out))
TEST_DEPS := $(TEST_EXES:.out=.d)
-include $(TEST_DEPS)


.PHONY: tests
tests: BUILD_FLAGS = -ggdb -DDEBUG
tests: build/tests/main.test.o $(TEST_EXES)


build/tests/main.test.o: tests/main.test.cpp
	$(compile)


build/tests/%.test.o: tests/%.test.cpp
	$(compile)


build/tests/%.test.out: build/tests/%.test.o build/tests/main.test.o
	$(link)


# Recompiles on change. Pass relative src= and optional out= on the command line.
.PHONY: watch-test
watch-test: out=build/$(basename $(src)).out
watch-test:
	@if [ ! -f "$(src)" ]; then                   \
		echo "file doesn't exist: $(src)";        \
	else                                          \
		while true; do                            \
			clear;                                \
			$(MAKE) $(out) --no-print-directory;  \
			                                      \
			echo "watching $(notdir $(src)) ..."; \
			inotifywait -qq -e modify $(src);     \
		done;                                     \
	fi


# ======================================================================================================================
# Docs
# ======================================================================================================================
.PHONY: docs
docs:
	$(MAKE) -C docs html


# ======================================================================================================================
# Misc
# ======================================================================================================================
.PHONY: clean-all clean-tests clean-docs
clean-all:
	rm -rf build/ gcm.cache/


clean-tests:
	rm -rf build/tests/


clean-docs:
	rm -rf build/docs/
