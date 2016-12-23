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

void get_file_md5 (char* filepath, unsigned char md5array[MD5_DIGEST_LENGTH]) {
    int file_descript;
    unsigned long file_size;
    char* file_buffer;
    
    file_descript = open(filepath, O_RDONLY);
    file_size = get_size_by_fd(file_descript);
    file_buffer = mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);
    MD5((unsigned char*) file_buffer, file_size, md5array);
    munmap(file_buffer, file_size); 
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
        fs->f_paths[i] = line;
        get_file_md5 (fs->f_paths[i], fs->f_md5[i]);
    }
    
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
