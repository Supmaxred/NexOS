int strequ(const char *s1, const char *s2) 
{
    while (*s1 && *s2) {
        if (*s1 != *s2)
            return 0;

        s1++;
        s2++;
    }

    return (!*s1 && !*s2) == 1;
}

int strlen(const char *str)
{
	const char *s;

	for (s = str; *s; ++s);
	return(s - str);
}