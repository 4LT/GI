#include <stdlib.h>
#include "scene.h"
#include "canvas.h"
#include "tone_mapping.h"

static const int SCREEN_W = 256;
static const int SCREEN_H = 256;

static const vec3_t CAM_POS = {{ 278.0, 273.0, -800.0 }};
static const vec3_t CAM_UP = {{ 0.0, 1.0, 0.0 }};
static const vec3_t CAM_LOOK = {{ 278.0, 273.0, 0.0 }};

void print_vec3(FILE *f, vec3_t vec)
{
    fprintf(f, "%f %f %f ", vec.v[0], vec.v[1], vec.v[2]);
}

void print_color(FILE *f, color_t color)
{
    fprintf(f, "%f %f %f ", color.c[0], color.c[1], color.c[2]);
}

void print_patch(FILE *f, vec3_t vert0, vec3_t vert1, vec3_t vert2,
        vec3_t vert3, color_t color)
{
    print_vec3(f, vert0);
    print_vec3(f, vert1);
    print_vec3(f, vert2);
    print_vec3(f, vert3);
    print_vec3(f, v3_normalize(v3_cross(
            v3_sub(vert1, vert0),
            v3_sub(vert2, vert0))));
    fputc('\n', f);
    fputs("0.0 0.0 0.0 ", f);
    print_color(f, color);
    fputc('\n', f);
}

void print_quad(FILE *f, vec3_t vert0, vec3_t vert1, vec3_t vert2, vec3_t vert3,
        color_t color, int subdiv)
{
    vec3_t vert_mat[subdiv+1][subdiv+1];
    for (int r = 0; r <= subdiv; r++) {
        vfloat_t v = (vfloat_t)r / subdiv;
        vec3_t subvert_1 = v3_blend(vert0, vert1, v);
        vec3_t subvert_2 = v3_blend(vert3, vert2, v);
        for (int c = 0; c <= subdiv; c++) {
            vfloat_t u = (vfloat_t)c / subdiv;
            vert_mat[r][c] = v3_blend(subvert_1, subvert_2, u);
        }
    }

    for (int r = 0; r < subdiv; r++)
    for (int c = 0; c < subdiv; c++) {
        print_patch(f, vert_mat[r][c], vert_mat[r][c+1],
                vert_mat[r+1][c+1], vert_mat[r+1][c], color);
    }

}

void print_head(FILE *f, vec3_t cam_pos, vec3_t cam_look, vec3_t cam_up)
{
    print_vec3(f, cam_pos);
    print_vec3(f, cam_look);
    print_vec3(f, cam_up);
    fputc('\n', f);
    fprintf(f, "%d %d %d %d %d \n", SCREEN_W, SCREEN_H, 256, 256, 100);
}

int main(int argc, char *argv[])
{
    vec3_t room_coords[] = {
        {{ 552.8, 0.0, 0.0 }},
        {{ 0.0, 0.0, 0.0 }},
        {{ 0.0, 0.0, 559.2 }},
        {{ 549.6, 0.0, 559.2 }},

        {{ 556.0, 548.8, 0.0 }},
        {{ 556.0, 548.8, 559.2 }},
        {{ 0.0, 548.8, 559.2 }},
        {{ 0.0, 548.8, 0.0 }}
    };

    vec3_t light_coords[] = {
        {{ 343.0, 548.8, 227.0 }},
        {{ 343.0, 548.8, 332.0 }},
        {{ 213.0, 548.8, 332.0 }},
        {{ 213.0, 548.8, 227.0 }}
    };

    color_t light = (color_t) {{ 1, 1, 1 }};
    color_t white = (color_t) {{ 0.4, 0.4, 0.4 }};
    color_t green = (color_t) {{ 0.0, 0.5, 0.0 }};
    color_t red = (color_t) {{ 0.5, 0.0, 0.0 }};
    color_t mag = (color_t) {{ .4, 0, .5 }};

    FILE *f = fopen("mymesh.txt", "w");

    print_head(f, CAM_POS, CAM_LOOK, CAM_UP);
    /* Floor */
    print_quad(f, room_coords[0],
            room_coords[1], room_coords[2], room_coords[3], white, 6);
    /* Walls */
    print_quad(f, room_coords[6],
            room_coords[5], room_coords[3], room_coords[2], white, 6);
    print_quad(f, room_coords[7],
            room_coords[6], room_coords[2], room_coords[1], green, 6);
    print_quad(f, room_coords[0],
            room_coords[3], room_coords[5], room_coords[4], red, 6);
    print_quad(f, room_coords[0],
            room_coords[4], room_coords[7], room_coords[1], mag, 6);
    /* Ceiling */
    print_quad(f, room_coords[4],
            room_coords[5], light_coords[1], light_coords[0], red, 4);
    print_quad(f, room_coords[4],
            light_coords[0], light_coords[3], room_coords[7], green, 4);
    print_quad(f, light_coords[1],
            room_coords[5], room_coords[6], light_coords[2], white, 4);
    print_quad(f, light_coords[3],
            light_coords[2], room_coords[6], room_coords[7], mag, 4);
    /* Light */
    print_quad(f, light_coords[0],
            light_coords[1], light_coords[2], light_coords[3], light, 3);

    fclose(f);
    return 0;
}
