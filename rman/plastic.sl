/* plastic.sl - simple plastic surface
 *
 * The Blue Moon Rendering Tools (BMRT) are:
 * (c) Copyright 1990-2000 Exluna, Inc. and Larry Gritz. All rights reserved.
 */

#define xor(a, b) ((a) && !(b) || !(a) && (b))

surface
plastic (float Ka = 1, Kd = 0.5, Ks = 0.5, roughness = 0.1;
	 color specularcolor = 0.5;)
{
    color white = color "rgb" (0.9, 0.9, 0.9);
    color red = color "rgb" (0.9, 0.0, 0.0);
    color samp;

    float theta, phi, r, thetaMax = PI/4, phiMax = PI/4;
    normal Nw = ntransform("world", N);
    float x = xcomp(Nw),
        y = ycomp(Nw),
        z = zcomp(Nw);
    r = sqrt(x*x + y*y + z*z);
    theta = atan(y, x);
    phi = acos(z / r);

    if (xor(mod(theta, thetaMax) < thetaMax/2, mod(phi, phiMax) < phiMax/2)) {
        samp = red;
    }
    else {
        samp = white;
    }
    normal Nf = faceforward (normalize(N),I);
    Ci = samp * (Ka*ambient() + Kd*diffuse(Nf)) +
	 specularcolor * Ks*specular(Nf,-normalize(I),roughness);
    Oi = Os;  Ci *= Oi;
}
