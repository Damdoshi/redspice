
#include		<unistd.h>

int			main(void)
{
  char			buffer[256];
  int			len;
  int			cnt;
  int			i;

  cnt = 1;
  while ((len = read(0, &buffer[0], sizeof(buffer))) > 0)
    {
      for (i = 0; i < len; ++i)
	buffer[i] ^= cnt++ & 0xFF;
      write(1, &buffer[0], len);
    }
  return (0);
}

