/*
 * Remove the http multipart header
 */
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>

#define HTTP_HDR_END      "\r\n\r\n"
#define HTTP_HDR_SEP      "\r\n"
#define HTTP_HDR_END_LEN  strlen(HTTP_HDR_END)
#define HTTP_HDR_CNTLEN   "Content-length: "


int get_ctn_len (char *buf)
{
  char *ptr1, *ptr2, *ctn_len;
  char len[10];
  
  ptr1 = strstr(buf, HTTP_HDR_CNTLEN);
  if (ptr1 == NULL) {
    return 0;
  }
  ptr2 = strstr(ptr1, HTTP_HDR_SEP);
  if (ptr2 == NULL) {
    return 0;
  }
  
  ctn_len = ptr1 + strlen(HTTP_HDR_CNTLEN);
  strncpy(len, ctn_len, ptr2 - ctn_len);
  return atoi(len);
}

int main (int argc, char **argv) 
{
  char   *input, *output;
  //char   *boundary;
  FILE   *p_in, *p_out;
  long   fsize;
  char   *buffer;
  size_t result;
  int    ctn_len;
  char   *p_hdr_start, *p_hdr_end, *p_ctn_start;
  //void  *p_bdry;
  
  // Get parameter
  input = argv[1];
  output = argv[2];
  //boundary = argv[3];
  
  // Open file
  p_in = fopen(input, "r");
  p_out = fopen(output, "a+");
  if (p_in == NULL || p_out == NULL) {
    printf("fopen() error");
    return -1;
  }
  
  // Obtain file size
  fseek (p_in, 0, SEEK_END);
  fsize = ftell(p_in);
  rewind(p_in);
  
  // Allocate memory to store file content
  buffer = malloc(fsize);
  if (buffer == NULL) {
    printf("malloc() error");
    return -1;
  }
  
  // Read file
  result = fread(buffer, 1, fsize, p_in);
  
  // Skip the multipart header and write to file
  p_hdr_start = buffer;
  while ((p_hdr_end = strstr(p_hdr_start, HTTP_HDR_END)) != NULL) {
    ctn_len = get_ctn_len(p_hdr_start);
    p_ctn_start = p_hdr_end + HTTP_HDR_END_LEN;
    fwrite(p_ctn_start, 1, ctn_len, p_out);
    p_hdr_start = p_ctn_start + ctn_len + HTTP_HDR_END_LEN;
  }
  
  // XXX: Bad method
  /*p_bdry = buffer;
  while ((p_hdr_end = strstr(p_bdry, HTTP_HDR_END)) != NULL) {
    p_ctn_start = p_hdr_end + HTTP_HDR_END_LEN;
    p_bdry = memmem(p_ctn_start, (size_t)fsize - ((char *)p_ctn_start - (char *)buffer), boundary, (size_t)strlen(boundary));
    if (p_bdry == NULL)
      break;
    fwrite(p_ctn_start, 1, p_bdry - p_ctn_start, p_out);
  }*/
  
  // Close file
  free(buffer);
  fclose(p_out);
  fclose(p_in);

  return 0;
}
