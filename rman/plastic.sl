/* plastic.sl - simple plastic surface
 *
 * The Blue Moon Rendering Tools (BMRT) are:
 * (c) Copyright 1990-2000 Exluna, Inc. and Larry Gritz. All rights reserved.
 */


surface
plastic (float Ka = 1, Kd = 0.5, Ks = 0.5, roughness = 0.1;
	 color specularcolor = 1;)
{
    normal Nf = faceforward (normalize(N),I);
    Ci = Cs * (Ka*ambient() + Kd*diffuse(Nf)) +
	 specularcolor * Ks*specular(Nf,-normalize(I),roughness);
    Oi = Os;  Ci *= Oi;
}
