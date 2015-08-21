GLFW = `pkg-config --cflags glfw3` `pkg-config --libs --static glfw3`
GLEW = `pkg-config --cflags glew` `pkg-config --libs glew`

LIB = -lSOIL
COMMON = shader.cpp optparse.cpp texture.cpp
TARGET = Mandelbrot-set

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	clang++ $(TARGET).cpp -o $(TARGET) $(COMMON) $(GLFW) $(GLEW) $(LIB)
