#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
#include "config.h"

#define MAXBUF 1024
#define DELIM "="
 
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}


size_t trimwhitespace2(char *out, size_t len, const char *str)
{
  if(len == 0)
    return 0;

  const char *end;
  size_t out_size;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
  {
    *out = 0;
    return 1;
  }

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < len-1 ? (end - str) : len-1;

  // Copy trimmed string and add null terminator
  memcpy(out, str, out_size);
  out[out_size] = 0;

  return out_size;
}

struct config get_config(char *filename)
{
        struct config configstruct;
        FILE *file = fopen (filename, "r");
 
        if (file != NULL)
        {
                char line[MAXBUF];
                int i = 0;
 
                while(fgets(line, sizeof(line), file) != NULL)
                {
						char *ch;
						char *confitem;
						ch = strtok(line, DELIM);
						while (ch != NULL) {
							if (confitem == NULL) {
								confitem = trimwhitespace(ch);
							} else {
								if (strcmp(confitem, "hostname") == 0) {
									trimwhitespace2(configstruct.hostname, MAXBUF, ch);
								} else if (strcmp(confitem, "port") == 0) {
									trimwhitespace2(configstruct.port, MAXBUF, ch);
								} else if (strcmp(confitem, "gametype") == 0) {
									trimwhitespace2(configstruct.gametype, MAXBUF, ch);
								}
							
								confitem = NULL;
							}
							ch = strtok(NULL, DELIM);
						}
                        i++;
                } // End while
        } // End if file
       
        fclose(file);   
       
        return configstruct;
 
}


