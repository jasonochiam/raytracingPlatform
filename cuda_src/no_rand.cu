#include <iostream>
#include <time.h>
#include <float.h>
#include <vector>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable_list.h"


__device__ vec3 color(const ray& ray, hitable **obj){
    hit_record rec;
    if ((*obj)->hit(ray, 0.0, FLT_MAX, rec)){
        // diffuse shading similar to CPU implementation
        vec3 light_dir = unit_vector(vec3(1.0f, 1.0f, -1.0f)); // light source 
        float diffuse = fmaxf(0.0f, dot(rec.normal, light_dir));
        
        // ? set sphere color
        vec3 sphere_color(0.7f, 0.2f, 0.2f); // Red sphere (R, G, B)
        
        // Add ambient light so it's not completely black in shadow
        float ambient = 0.35f;
        return sphere_color * (ambient + (1.0f - ambient) * diffuse);
    }
    else {
        vec3 unit_direction = unit_vector(ray.direction());
        float t = 0.5f*(unit_direction.y() + 1.0f);
        return (1.0f-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.2, 0.3, 0.5); // adjust backgroun gradient here 
    }
}

__global__ void render(vec3 *fb, int max_x, int max_y,
                       vec3 lower_left_corner, vec3 horizontal, vec3 vertical, vec3 origin,
                       hitable **world) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if((i >= max_x) || (j >= max_y)) return;
    int pixel_index = j*max_x + i;
    float u = float(i) / float(max_x);
    float v = float(j) / float(max_y);
    ray r(origin, lower_left_corner + u*horizontal + v*vertical);
    fb[pixel_index] = color(r, world);
}

__global__ void create_world(hitable **d_list, hitable **d_world) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *(d_list)   = new sphere(vec3(0,0,-1), 0.5);
        *d_world    = new hitable_list(d_list,1);
    }
}

__global__ void free_world(hitable **d_list, hitable **d_world) {
    delete *(d_list);
    delete *d_world;
}

int main() {
    int nx = 1200;
    int ny = 600;
    int tx = 8;
    int ty = 8;

    std::cerr << "Rendering a " << nx << "x" << ny << " image ";
    std::cerr << "in " << tx << "x" << ty << " blocks.\n";

    int num_pixels = nx*ny;
    size_t fb_size = num_pixels*sizeof(vec3);

    // allocate FB
    vec3 *fb;
    cudaMallocManaged((void **)&fb, fb_size);

    // make our world of hitables
    hitable **d_list;
    cudaMalloc((void **)&d_list, 1*sizeof(hitable *));
    hitable **d_world;
    cudaMalloc((void **)&d_world, sizeof(hitable *));
    create_world<<<1,1>>>(d_list,d_world);
    cudaGetLastError();
    cudaDeviceSynchronize();

    clock_t start, stop;
    start = clock();
    // Render our buffer
    dim3 blocks(nx/tx+1,ny/ty+1);
    dim3 threads(tx,ty);
    render<<<blocks, threads>>>(fb, nx, ny,
                                vec3(-2.0, -1.0, -1.0),
                                vec3(4.0, 0.0, 0.0),
                                vec3(0.0, 2.0, 0.0),
                                vec3(0.0, 0.0, 0.0),
                                d_world);
    cudaGetLastError();
    cudaDeviceSynchronize();
    stop = clock();
    double timer_seconds = ((double)(stop - start)) / CLOCKS_PER_SEC;
    std::cerr << "took " << timer_seconds << " seconds.\n";

    // Convert vec3 framebuffer to unsigned char array for PPM
    std::vector<unsigned char> pixels(nx * ny * 3);
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            size_t pixel_index = j*nx + i;
            int write_index = ((ny-1-j)*nx + i) * 3;
            
            vec3 col = fb[pixel_index];
            pixels[write_index + 0] = (unsigned char)(255.99 * col.r());
            pixels[write_index + 1] = (unsigned char)(255.99 * col.g());
            pixels[write_index + 2] = (unsigned char)(255.99 * col.b());
        }
    }

    // Write PPM
    std::FILE *out = std::fopen("output1.ppm", "wb");
    if (!out){
        std::cerr << "Failed to open output1.ppm for writing" << std::endl;
        return 1;
    }
    std::fprintf(out, "P6\n%d %d\n255\n", nx, ny);
    std::fwrite(pixels.data(), 1, pixels.size(), out);
    std::fclose(out);

    std::cout << "Wrote output1.ppm (" << nx << "x" << ny << ")" << std::endl;

    // clean up
    cudaDeviceSynchronize();
    free_world<<<1,1>>>(d_list,d_world);
    cudaGetLastError();
    cudaFree(d_list);
    cudaFree(d_world);
    cudaFree(fb);

    // useful for cuda-memcheck --leak-check full
    cudaDeviceReset();
}