#ifndef _string_h
#define _string_h

char* itoa(int i)
{
	return i + "0";
}

char* strchr (const char *s, int c)
{
  do {
    if (*s == c)
      {
        return (char*)s;
      }
  } while (*s++);
  return (0);
}

char* strcat(char *dest, const char *src)
{
    char *rdest = dest;

    while (*dest)
      dest++;
    while (*dest++ = *src++)
      ;
    return rdest;
}

#endif
