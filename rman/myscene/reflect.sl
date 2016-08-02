surface reflect(
    float minr = 0.55, maxr = 1,
    Ka = 1, Kd = 0.5, roughness = 0.07)
{
    vector In = normalize(I);
    vector Nn = normalize(N);
    float dotNI = Nn.In;
    float rscale = ((1+dotNI) * maxr - dotNI *  minr);
    Ci = 0;

    if (dotNI < 0) {
        vector r = reflect(In, Nn);
        Ci+= rscale * (trace(P, r));
    }

    Oi = Os;
    Ci+= Cs * (Ka*ambient() + Kd*diffuse(Nn)) + rscale*specular(Nn, -In, roughness);
}
