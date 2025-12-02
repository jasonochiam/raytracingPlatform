CUDA_PATH     ?= /usr/
HOST_COMPILER  = g++
NVCC           = $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)

NVCCFLAGS      = -m64
GENCODE_FLAGS  = -gencode arch=compute_89,code=sm_89 # 89 is for ryan's ada arch

SRCS = no_rand.cu # change to no_rand.cu for no anti-aliasing, change to main.cu otherwise
INCS = vec3.h ray.h hitable.h hitable_list.h sphere.h camera.h

raytracer_cuda: raytracer_cuda.o
	$(NVCC) $(NVCCFLAGS) $(GENCODE_FLAGS) -o raytracer_cuda raytracer_cuda.o

raytracer_cuda.o: $(SRCS) $(INCS)
	$(NVCC) $(NVCCFLAGS) $(GENCODE_FLAGS) -o raytracer_cuda.o -c $(SRCS)

out.ppm: raytracer_cuda
	rm -f out.ppm
	./raytracer_cuda > out.ppm

out.jpg: out.ppm
	rm -f out.jpg
	ppmtojpeg out.ppm > out.jpg

profile_basic: raytracer_cuda
	nvprof ./raytracer_cuda > out.ppm

# use nvprof --query-metrics
profile_metrics: raytracer_cuda
	nvprof --metrics achieved_occupancy,inst_executed,inst_fp_32,inst_fp_64,inst_integer ./raytracer_cuda > out.ppm

clean:
	rm -f raytracer_cuda raytracer_cuda.o out.ppm out.jpg