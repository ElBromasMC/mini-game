CXX = clang++
CXXFLAGS = -Wall -Wextra -O0 $(shell pkg-config --cflags glut)
LDFLAGS = $(shell pkg-config --libs glut) -lGL -lGLU

BUILD_DIR := ./build

live:
	watchexec --ignore "$(BUILD_DIR)" --exts cpp,h,hpp -r \
		'make "$(BUILD_DIR)/main" && "./$(BUILD_DIR)/main"'

$(BUILD_DIR)/%: %.cpp debug.cpp
	@mkdir -p "$(BUILD_DIR)"
	$(CXX) $(CXXFLAGS) -o "$@" "$<" $(LDFLAGS)

clean:
	rm -f build/*

.PHONY: live clean

