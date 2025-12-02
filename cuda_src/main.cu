#include <iostream>
#include <time.h>
#include <float.h>
#include <curand_kernel.h>
#include <vector>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"

#define IMAGEX 800
#define IMAGEY 600
#define FOV 90

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

__global__ void render_init(int max_x, int max_y, curandState *rand_state) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if((i >= max_x) || (j >= max_y)) return;
    int pixel_index = j*max_x + i;
    //Each thread gets same seed, a different sequence number, no offset
    curand_init(2000, pixel_index, 0, &rand_state[pixel_index]);
}

__global__ void render(vec3 *fb, int max_x, int max_y, int ns, camera **cam, hitable **world, curandState *rand_state) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if((i >= max_x) || (j >= max_y)) return;
    int pixel_index = j*max_x + i;
    curandState local_rand_state = rand_state[pixel_index];
    vec3 col(0,0,0);
    for(int s=0; s < ns; s++) {
        float u = float(i + curand_uniform(&local_rand_state)) / float(max_x);
        float v = float(j + curand_uniform(&local_rand_state)) / float(max_y);
        ray r = (*cam)->get_ray(u,v);
        col += color(r, world);
    }
    fb[pixel_index] = col/float(ns);
}

__global__ void create_world(hitable **d_list, hitable **d_world, camera **d_camera) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *(d_list)   = new sphere(vec3(0,0,-1), 0.5);
        *d_world    = new hitable_list(d_list,1);
        *d_camera   = new camera();
    }
}

__global__ void free_world(hitable **d_list, hitable **d_world, camera **d_camera) {
    delete *(d_list);
    delete *d_world;
    delete *d_camera;
}

int main() {
    int nx = 1200;
    int ny = 600;
    int ns = 100;
    int tx = 8;
    int ty = 8;

    std::cerr << "Rendering a " << nx << "x" << ny << " image with " << ns << " samples per pixel ";
    std::cerr << "in " << tx << "x" << ty << " blocks.\n";

    int num_pixels = nx*ny;
    size_t fb_size = num_pixels*sizeof(vec3);

    // allocate everything
    vec3 *fb;
    cudaMallocManaged((void **)&fb, fb_size);

    curandState *d_rand_state;
    cudaMalloc((void **)&d_rand_state, num_pixels*sizeof(curandState));


    hitable **d_list;
    hitable **d_world;
    camera **d_camera;
    cudaMalloc((void **)&d_list, 1*sizeof(hitable *));
    cudaMalloc((void **)&d_world, sizeof(hitable *));
    cudaMalloc((void **)&d_camera, sizeof(camera *));
    
    // set stack limit, not sure if this is necessary?
    cudaDeviceSetLimit(cudaLimitStackSize, 4096);
    
    create_world<<<1,1>>>(d_list,d_world,d_camera);
    cudaDeviceSynchronize();

    clock_t start, stop;
    start = clock();
    // Render our buffer
    dim3 blocks(nx/tx+1,ny/ty+1);
    dim3 threads(tx,ty);

    render_init<<<blocks, threads>>>(nx, ny, d_rand_state);
    cudaDeviceSynchronize();
    render<<<blocks, threads>>>(fb, nx, ny,  ns, d_camera, d_world, d_rand_state);
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

    free_world<<<1,1>>>(d_list,d_world,d_camera);
    cudaDeviceSynchronize();
    
    cudaFree(d_list);
    cudaFree(d_world);
    cudaFree(d_camera);
    cudaFree(d_rand_state);
    cudaFree(fb);

    cudaDeviceReset();
}