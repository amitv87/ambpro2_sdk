#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t byte;
typedef uint8_t uchar;
typedef uint32_t uint;
typedef uint32_t ulong;

static int mode = 0;
static char* in_filename = NULL;
static char* out_filename = NULL;

void show_help(char *self){
  printf("Realtek IOT checksum v%d.%d.%d\n\r",1,1,0);
  printf("Date: %s\n\r","Jul 12 2022");
  printf("%s -m fcs -o output_name filename\n\r", self);
  printf("-m : mode, dft mode is attaching checksum to file end\n\r");
  printf("           fcs mode is for fcs image format\n\r");
  printf("-o : customized output filename, using filename as input/output if this not present\n\r");
  return;
}

int parse_option(int param_1, char** param_2){
  char cVar1;
  int iVar2;
  int local_c;

  local_c = 1;
  do {
    if (param_1 <= local_c) {
      if (out_filename == 0) {
        out_filename = in_filename;
      }
      printf("input  : %s\n\r",in_filename);
      printf("otuput : %s\n\r",out_filename);
      printf("mode   : %d\n\r",mode);
      return 0;
    }
    char* param = param_2[local_c];
    char* next_param = param_2[local_c + 1];
    if (param[0] == '-') {
      if (param[1] == 'm') {
        iVar2 = strcmp(next_param, "fcs");
        if (iVar2 == 0) {
          mode = 1;
        }
        else {
          iVar2 = strcmp(next_param, "dft");
          if (iVar2 != 0) {
            return -1;
          }
          mode = 0;
        }
      }
      else {
        if (cVar1 != 'o') {
          return -1;
        }
        out_filename = next_param;
      }
      local_c = local_c + 1;
    }
    else {
      in_filename = param;
    }
    local_c = local_c + 1;
  } while( true );
}


int main(int argc,char **argv){
  byte bVar1;
  long lVar2;
  int iVar3;
  FILE *__stream;
  FILE *__stream_00;
  ulong uVar4;
  byte *__s;
  byte *pbVar5;
  char *pcVar6;
  size_t __size;
  long in_FS_OFFSET;
  uint checksum;

  lVar2 = *(long *)(in_FS_OFFSET + 0x28);
  checksum = 0;
  iVar3 = parse_option(argc,argv);
  if (iVar3 < 0) {
    show_help(*argv);
    iVar3 = 1;
    goto LAB_00101364;
  }
  __stream = fopen(in_filename,"rb+");
  pcVar6 = in_filename;
  if (__stream == (FILE *)0x0) {
LAB_00101412:
    printf("cannot open %s\n\r",pcVar6);
    iVar3 = -2;
  }
  else {
    if (in_filename == out_filename) {
      fseek(__stream,0,2);
      uVar4 = ftell(__stream);
      fseek(__stream,0,0);
      __size = (size_t)((int)uVar4 + 100);
      __s = (byte *)malloc(__size);
      __stream_00 = __stream;
      if (__s == (byte *)0x0) {
LAB_00101441:
        fclose(__stream);
        iVar3 = -3;
        goto LAB_00101364;
      }
    }
    else {
      __stream_00 = fopen(out_filename,"wb");
      pcVar6 = out_filename;
      if (__stream_00 == (FILE *)0x0) goto LAB_00101412;
      fseek(__stream,0,2);
      uVar4 = ftell(__stream);
      fseek(__stream,0,0);
      __size = (size_t)((int)uVar4 + 100);
      __s = (byte *)malloc(__size);
      if (__s == (byte *)0x0) {
        if (__stream != __stream_00) {
          fclose(__stream_00);
        }
        goto LAB_00101441;
      }
    }
    iVar3 = (int)(uVar4 & 0xffffffff);
    printf("size = %d \n\r",uVar4 & 0xffffffff);
    memset(__s,0,__size);
    fread(__s,1,(long)iVar3,__stream);
    if (0 < iVar3) {
      pbVar5 = __s;
      do {
        bVar1 = *pbVar5;
        pbVar5 = pbVar5 + 1;
        checksum = checksum + bVar1;
      } while (pbVar5 != __s + (ulong)(iVar3 - 1) + 1);
    }
    printf("checksum %x\n\r",checksum);
    if (mode == 0) {
      fseek(__stream_00,0,2);
      fwrite(&checksum,4,1,__stream_00);
    }
    else if (mode == 1) {
      fseek(__stream_00,0,0);
      uint32_t fcsd = 0x44534346;
      fwrite(&fcsd, sizeof(fcsd), 1, __stream_00);
      fwrite(&checksum, sizeof(checksum), 1, __stream_00);
      fwrite(__s,(long)iVar3,1,__stream_00);
    }
    free(__s);
    if (__stream != __stream_00) {
      fclose(__stream_00);
    }
    fclose(__stream);
    iVar3 = 0;
  }
LAB_00101364:
  if (lVar2 != *(long *)(in_FS_OFFSET + 0x28)) fprintf(stderr, "stack_chk_fail");
  return iVar3;
}
