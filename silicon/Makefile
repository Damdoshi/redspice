CXX ?= g++
BUNNY_CXX ?= b++
CXXFLAGS ?= -O2 -g -Wall -Wextra -Wno-misleading-indentation -std=c++17
CORE = src/Document.cpp src/Extract.cpp src/Hierarchy.cpp src/Simulator.cpp src/Examples.cpp src/Image.cpp src/Editor.cpp
HEADERS = include/Silicon.hpp include/Editor.hpp include/ComponentDock.hpp
.PHONY: all cli test clean examples
all:
	$(MAKE) -C .. all
cli: ../bcad-ic-cli
../bcad-ic-cli: $(CORE) src/Main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -DREDSILICON_CLI -Iinclude $(CORE) src/Main.cpp -o $@
test: $(CORE) tests/core.cpp tests/window.cpp tests/hierarchy.cpp src/Window.cpp ../tests/support/lapin.h $(HEADERS)
	mkdir -p .test-build
	$(CXX) -O1 -g -std=c++17 -Wall -Wextra -Werror=missing-field-initializers -Werror=cast-function-type -Wno-misleading-indentation -fsanitize=address,undefined -fno-omit-frame-pointer -no-pie -DREDSILICON_TEST_C_API -Iinclude -I../tests/support $(CORE) tests/core.cpp tests/window.cpp tests/hierarchy.cpp -o .test-build/core
	ASAN_OPTIONS=detect_leaks=$${REDSILICON_DETECT_LEAKS:-1}:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 .test-build/core
examples: cli
	../bcad-ic-cli --examples examples
clean:
	rm -rf .test-build
	rm -f ../bcad-ic-cli
