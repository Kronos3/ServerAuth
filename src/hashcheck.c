/*
 * hashcheck.c
 * 
 * Copyright 2016 Andrei Tumbar <atuser@Kronos>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <hashcheck.h>

#define MAX_FILES 32

struct file_structure * main_f_struct;

int ar_index (char** in_arr, char* to_index)
{
    int i;
    for (i=0; in_arr[i]; i++)
    {
        if (strcmp(in_arr[i], to_index)==0)
        {
            return i;
        }
    }
    return -1;
}

unsigned long get_size_by_fd(int fd) {
    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) exit(-1);
    return statbuf.st_size;
}

void get_file_md5 (char* filename, unsigned char ss[MD5_DIGEST_LENGTH]) {
    int n;
    MD5_CTX c;
    char buf[512];
    ssize_t bytes;
    unsigned char out[MD5_DIGEST_LENGTH];

    MD5_Init(&c);
    removechar (filename, '\n');
    FILE * fp = fopen (filename, "rb");
    while((bytes=fread(buf, 1, 512, fp)) != 0)
    {
        MD5_Update(&c, buf, bytes);
    }
    fclose (fp);

    MD5_Final(out, &c);
    int i;
    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(ss+i, "%02x", out[i]);
}

/*
 * hash_conf: path to file with a filename per line
*/
void hash_init (char* hash_conf, struct file_structure * fs) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    fp = fopen(hash_conf, "r");
    
    int i;
    for (i = 0; (read = getline(&line, &len, fp)) != -1; i++) {
        if (strcmp (line, "\0")==0)
            continue;
        fs->f_paths[i] = malloc (sizeof (char) * strlen(line));
        fs->f_md5[i] = malloc (sizeof (char) * 32);
        strcpy(fs->f_paths[i], line);
        fs->f_paths[i][strlen(fs->f_paths[i])-1] = 0;
        get_file_md5 (line, fs->f_md5[i]);
    }
    
    fclose(fp);
}

void dump_md5 (char* file, struct file_structure * fs) {
    FILE * fp = fopen (file, "w+b");
    fwrite (fs, sizeof(struct file_structure), 1, fp);
    fclose (fp);
}

void removechar( char str[], char t )
{
  int i,j;
  for(i=0; i<strlen(str); i++)
  {
    if (str[i]==t)
      for (j=i; j<strlen(str); j++)
      {
        str[j]=str[j+1];   
      } 
  }
}

void get_md5 (char* file, struct file_structure * fs) {
    FILE * fp = fopen(file, "rb");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    
    fread (fs, sizeof(struct file_structure), 1, fp);
    fclose(fp);
}

int check_hash (struct file_structure * fs) {
    int i;
    for (i=0; fs->f_md5[i]; i++) {
        unsigned char buff[MD5_DIGEST_LENGTH];
        get_file_md5 (fs->f_paths[i], buff);
        if (strcmp(buff, fs->f_md5[i]))
        {
            return 0;
        }
    }
    return 1;
}
