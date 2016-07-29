surface
rim (float Ka = 1, Kd = 0.5, rimexp = 10, rimscale = 1; )
{
    normal Nn = normalize(N);
    vector In = normalize(I);
    color Cpl = 0;
    illuminance(P, Nn, PI) {
        vector Ln = normalize(L);
        float LNdot = Ln.Nn;
        float LIdot = Ln.In;
        if (LNdot > 0) {
            Cpl+= Cl * LNdot;
        }
        if (LIdot > 0) {
            Cpl+= Cl * pow(length(Ln^Nn) * LIdot, rimexp) * rimscale;
        }

    }
    Ci = Cs * (Ka*ambient() + Kd*Cpl);
    Oi = Os;  Ci *= Oi;
}
