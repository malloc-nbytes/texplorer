#ifndef AP_H
#define AP_H

#include <stdint.h>
#include <string.h>

#define AP_1HYPH_OK     1  // One hyphen with an argument(s). Ex: -a or -abc
#define AP_2HYPH_OK     2  // Two hyphens with an argument. Ex: --foo
#define AP_0HYPH_OK     4  // Just an argument with no hyphens. Ex: s = foo
#define AP_EMPTY        8  // Empty string. Ex: s = "".
#define AP_INVALID      16 // Fatal, nothing. Ex: s = NULL.
#define AP_1HYPH_NO_ARG 32 // Contains 1 hyphen and no arguments. Ex: s = -
#define AP_2HYPH_NO_ARG 64 // Contains 2 hyphens and no arguments. Ex: s = --

#define AP_CHECK_0HYPH_OK(arg) (((arg.status) & AP_0HYPH_OK) != 0)
#define AP_CHECK_1HYPH_OK(arg) (((arg.status) & AP_1HYPH_OK) != 0)
#define AP_CHECK_2HYPH_OK(arg) (((arg.status) & AP_2HYPH_OK) != 0)
#define AP_CHECK_NOT_OK(arg) ((((arg.status) & AP_EMPTY) != 0) || (((arg.status) & AP_INVALID) != 0))
#define AP_CHECK_NO_ARG(arg) ((((arg.status) & AP_1HYPH_NO_ARG) != 0) || (((arg.status) & AP_2HYPH_NO_ARG) != 0))

struct Arg {
  const char *value;
  size_t len;
  uint32_t status;
};

#ifdef AP_IMPL

static const char *__ap_prog_name = NULL;

struct Arg ap_parse(char *str, char delim)
{
  struct Arg arg;
  arg.value = NULL;
  arg.len = 0;
  arg.status = 0;

  if (!str) {
    arg.status |= AP_INVALID;
    return arg;
  }

  size_t len = strlen(str);

  if (len == 0) {
    arg.status |= AP_EMPTY;
  }
  else if (len == 1 && *str == delim) {
    arg.status |= AP_1HYPH_NO_ARG;
  }
  else if (len == 2 && str[0] == delim && str[1] == delim) {
    arg.status |= AP_2HYPH_NO_ARG;
  }
  else if (str[0] == delim && str[1] == delim) {
    arg.status |= AP_2HYPH_OK;
    arg.len = len-2;
    arg.value = str+2;
  }
  else if (str[0] == delim) {
    arg.status |= AP_1HYPH_OK;
    arg.len = len-1;
    arg.value = str+1;
  }
  else {
    arg.status |= AP_0HYPH_OK;
    arg.len = len;
    arg.value = str;
  }

  return arg;
}

char *ap_eat(int *argc, char ***argv)
{
  static int prog = 0;
  if (!prog) {
    __ap_prog_name = *(*argv);
    prog = 1;
  }
  if (*argc == 0) {
    return NULL;
  }
  (*argc)--;
  return *(*argv)++;
}

const char *ap_prog_name(void)
{
  return __ap_prog_name;
}

#endif // AP_IMPL

#endif // AP_H
