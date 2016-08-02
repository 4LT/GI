surface
rim (float Ka = 1, Kd = 0.5, Krim = 1, rimexp = 10, peek = 0.1; )
{
    normal Nn = normalize(N);
    vector In = normalize(I);
    point Ppeek = P + Nn * peek;
    color Crim = 0;
    illuminance(Ppeek, Nn, PI) {
        vector Ln = normalize(L);
        float LIdot = Ln.In;
        if (LIdot > 0) {
            Crim+= Cl * pow(length(Ln^Nn) * LIdot, rimexp);
        }

    }

    Ci = Cs * (Ka*ambient() + Kd*diffuse(Nn) + Krim*Crim);
    Oi = Os;  Ci *= Oi;
}
