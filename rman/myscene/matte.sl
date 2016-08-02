surface
matte (float Ka = 1, Kd = 0.5; )
{
    normal Nf = faceforward (normalize(N),I);
    Ci = Cs * (Ka*ambient() + Kd*diffuse(Nf)); 
    Oi = Os;  Ci *= Oi;
    if (N.I > 0) {
        color cyan = color "rgb" (0.0, 1.0, 1.0);
        Oi = Os; Ci = cyan;
    }
}
