/*****************************************
 Yleiset matikka määritykset
******************************************/

#define PI 3.14159

// Palauttaa liukuluku osuuden
static inline float frac(float value)
{
    // Palautetaan arvo
    return (value - (uint32_t)value);
}