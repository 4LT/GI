imager
imager_spec(float Ks = 0.6, roughness = 0.02)
{
    vector In = normalize(I);
    Ci+= Ks*specular(In, In, roughness);
}
