#include <stdio.h>
#include <stdlib.h>
// get access to system information
#include <sys/utsname.h>


int main()
{
  // Team information
  int teamSize = 1;
  char member[] = "Shayan Amirhosseini";
  printf("*************************************************\n");
  printf("Team size: %d\n", teamSize);
  printf("Team members:\n");
  printf("%s\n", member);
  // Getting system information
  struct utsname sysInfo;
  if (uname(&sysInfo) < 0)
  {
    perror("uname");
    exit(EXIT_FAILURE);
  }
  printf("*************************************************\n");
  printf("System name: %s\n", sysInfo.sysname);
  printf("Node name: %s\n", sysInfo.nodename);
  printf("Realease: %s\n", sysInfo.release);
  printf("Machine: %s\n", sysInfo.machine);
  printf("Version: %s\n", sysInfo.version);
  printf("*************************************************\n");
  return (EXIT_SUCCESS);
}