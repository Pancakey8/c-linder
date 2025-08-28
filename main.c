#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define WIDTH 60
#define HEIGHT 60

typedef struct {
  float x, y, z;
} vec3f_t;

vec3f_t vec3f_add(vec3f_t v, vec3f_t w) {
  return (vec3f_t){.x = v.x + w.x, .y = v.y + w.y, .z = v.z + w.z};
}

vec3f_t vec3f_scale(vec3f_t v, float scale) {
  return (vec3f_t){.x = v.x * scale, .y = v.y * scale, .z = v.z * scale};
}

float vec3f_dot(vec3f_t v, vec3f_t w) {
  return v.x * w.x + v.y * w.y + v.z * w.z;
}

typedef struct {
  vec3f_t front, up, right; // Bases
  vec3f_t center;           // Position
} camera_t;

vec3f_t camera_transform(camera_t cam, vec3f_t v) {
  vec3f_t tr = vec3f_scale(cam.center, -1);
  vec3f_t P = vec3f_add(v, tr);
  float x = vec3f_dot(P, cam.right);
  float y = vec3f_dot(P, cam.up);
  float z = vec3f_dot(P, cam.front);
  return (vec3f_t){x, y, z};
}

vec3f_t rotate_y(vec3f_t v, float ang) {
  vec3f_t rx = {cosf(ang), 0, sinf(ang)};
  // row 2 simply preserves the y coordinate.
  vec3f_t rz = {-sinf(ang), 0, cosf(ang)};
  return (vec3f_t){vec3f_dot(rx, v), v.y, vec3f_dot(rz, v)};
}

int main(void) {
  float screen[WIDTH][HEIGHT] = {0}; // at (3,0,0) facing origin
  camera_t camera = {.center = {3, 0, 0},
                     .front = {-1, 0, 0},
                     .up = {0, 1, 0},
                     .right = {0, 0, -1}};
  char gradient[] = {'@', '%', '#', '*', '+', '=', '-', ':', '.'};

  float angle = 0;
  while (1) {
    for (size_t x = 0; x < WIDTH; ++x) {
      for (size_t y = 0; y < HEIGHT; ++y) {
        screen[x][y] = -1; // Since depth >0 are in vision, this is a safe pick.
      }
    }
    float min_depth = FP_INFINITE, max_depth = -FP_INFINITE;
    for (float theta = 0; theta <= 2 * M_PI; theta += 0.1) {
      for (float z = -1; z <= 1; z += 0.1) {
        vec3f_t world_p = {cosf(theta), sinf(theta), z};
        vec3f_t rot_p = rotate_y(world_p, angle);
        vec3f_t cam_p = camera_transform(camera, rot_p);
        int x = (cam_p.x + 1) * WIDTH / 3 + 10;
        int y = (cam_p.y + 1) * HEIGHT / 3;
        if (cam_p.z <= 0)
          continue;
        if (cam_p.z < min_depth)
          min_depth = cam_p.z;
        if (cam_p.z > max_depth)
          max_depth = cam_p.z;
        if (0 <= x && x < WIDTH && 0 <= y && y < HEIGHT) {
          if (screen[x][y] == -1 || screen[x][y] > cam_p.z)
            screen[x][y] = cam_p.z;
        }
      }
    }
    if (min_depth < 0)
      min_depth = 0;
    printf("\033[2J");
    for (size_t x = 0; x < WIDTH; ++x) {
      for (size_t i = 0; i < 10; ++i)
        putchar(' ');
      for (size_t y = 0; y < HEIGHT; ++y) {
        if (screen[x][y] >= 0) {
          int index = (screen[x][y] - min_depth) * (sizeof(gradient) - 1) /
                          (max_depth - min_depth) +
                      0.5f;
          putchar(gradient[index]);
          // printf("%1f ", screen[x][y]);
        } else {
          putchar(' ');
        }
      }
      putchar('\n');
    }
    // printf("Minimum: %f, Maximum: %f\n", min_depth, max_depth);
    angle += 0.1;
    angle = fmodf(angle, 2 * M_PI);
    usleep(50000);
  }
  return 0;
}
