GLFW = `pkg-config --cflags glfw3` `pkg-config --libs --static glfw3`
GLEW = `pkg-config --cflags glew` `pkg-config --libs glew`

COMMON = shader.cpp optparse.cpp
TARGET = Mandelbrot-set

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	clang++ $(TARGET).cpp -o $(TARGET) $(COMMON) $(GLFW) $(GLEW)
