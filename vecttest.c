#include <stdio.h>
#include "vecmatops.h"

void print_vec(struct vec3 v)
{
    printf("RECEIVED: %f, %f, %f\n\n", v.v[0], v.v[1], v.v[2]);
}

int main(char *args[])
{
    struct vec3 vec_x = {{ 1, 0, 0 }};
    struct vec3 vec_y = {{ 0, 1, 0 }};

    struct vec3 dot_ident = {{ 1, 1, 1 }};
    struct vec3 vec_test  = {{ 2, 3, 4 }};
    struct vec3 v;
    struct mat4 m;

    printf("dot:\nEXPECTED: 9\n");
    printf("RECEIVED: %f\n\n", v3_dot(vec_test, dot_ident));

    printf("cross:\nEXPECTED: 0, 0, 1\n");
    print_vec(v3_cross(vec_x, vec_y));
    
    printf("magnitude:\nEXPECTED: %f\n", sqrt(29.0));
    printf("RECEIVED: %f\n\n", v3_magnitude(vec_test));

    printf("scale:\nEXPECTED: 1, 4, 6\n");
    m = (struct mat4){ {{ 1, 0, 0, -1 },
                        { 0, 1, 0,  1 },
                        { 0, 0, 1,  2 },
                        { 0, 0, 0,  1 }} };
    print_vec(m4v3_transform(m, vec_test));

    printf("scale:\nEXPECTED: 6, 1.5, 6\n");
    m = (struct mat4){ {{ 3, 0,   0,   0 },
                        { 0, 0.5, 0,   0 },
                        { 0, 0,   1.5, 0 },
                        { 0, 0,   0,   1 }} };
    print_vec(m4v3_transform(m, vec_test));

    return 0;
}
