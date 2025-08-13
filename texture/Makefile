
all: a.out memory texture

a.out: main.cu
	nvcc $< -o $@

memory: memory.cu
	nvcc $< -o $@

texture: texture.cu
	nvcc $< -o $@

clean:
	rm a.out memory texture
