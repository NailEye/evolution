
static inline size_t strlen(const char *str)
{
    // Palautettava pituus
    size_t size = 0;
    // Loopataan merkit, kunnes tulee vastaan NUL-terminate
    for(size = 0; str[size] != '\0'; size++); 
    // Palautetaan pituus
    return size;
}
