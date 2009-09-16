/*
 *  openingbnr.c
 *
 *  Extract opening.bnr for sound.bin
 *
 *  Thanks to Magicus <magicus@gmail.com> and GX Team
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include <ogcsys.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fat.h>
#include "openingbnr.h"
#include "MD5.h"
#include "utils.h"
#include "TrackedMemoryManager.h"
//#define DEB_IT

static FILE *fp;

typedef struct {
    u8 zeroes[0x40];
    u32 imet; // "IMET"
    u8 zero_six_zero_three[8];  // fixed, unknown purpose
    u32 sizes[3];
    u32 flag1;
    u16 name_jp[0x2a]; // might be empty
    u16 name_en[0x2a];
    u16 name_de[0x2a];
    u16 name_fr[0x2a];
    u16 name_es[0x2a];
    u16 name_it[0x2a];
    u16 name_nl[0x2a];
    u8 zeroes_2[0x348];
    u8 crypto[0x10];
} imet_data_t;

typedef struct
{
  u16 type;
  u16 name_offset;
  u32 data_offset; // == absolut offset från U.8- headerns början
  u32 size; // last included file num for directories
} U8_node;

typedef struct
{
  u32 tag; // 0x55AA382D "U.8-"
  u32 rootnode_offset; // offset to root_node, always 0x20.
  u32 header_size; // size of header from root_node to end of string table.
  u32 data_offset; // offset to data -- this is rootnode_offset + header_size, aligned to 0x40.
  u8 zeroes[16];
} U8_archive_header;


typedef struct {
  u32 imd5_tag; // 0x494D4435 "IMD5";
  u32 size;  // size of the rest of part B, starting from next field.
  u8 zeroes[8];
  u8 md5[16];
  u32 payload_tag; // 0x4C5A3737 "LZ77" if this is lz77
  u32 payload_data;
}  imd5_header_t;


typedef struct {
	u32 tag;
} sound_header;	


u16 be16(const u8 *p)
{
	return (p[0] << 8) | p[1];
}


u32 be32(const u8 *p)
{
	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}


void md5(u8 *data, u32 len, u8 *hash)
{
	MD5(hash, data, len);
}


static void write_file(void* data, size_t size, char* name)
{
	FILE *out;
	out = fopen(name, "wb");
	fwrite(data, 1, size, out);
	fclose(out);
}


u8* decompress_lz77(u8 *data, size_t data_size, size_t* decompressed_size)
{
	u8 *data_end;
	u8 *decompressed_data;
	size_t unpacked_size;
	u8 *in_ptr;
	u8 *out_ptr;
	u8 *out_end;

	in_ptr = data;
	data_end = data + data_size;

	// Assume this for now and grow when needed
	unpacked_size = data_size;

	decompressed_data = CFMalloc(unpacked_size);
	out_end = decompressed_data + unpacked_size;

	out_ptr = decompressed_data;

	while (in_ptr < data_end) {
	  int bit;
	  u8 bitmask = *in_ptr;

	  in_ptr++;
	  for (bit = 0x80; bit != 0; bit >>= 1) {
	    if (bitmask & bit) {
	      // Next section is compressed
	      u8 rep_length;
	      u16 rep_offset;

	      rep_length = (*in_ptr >> 4) + 3;
	      rep_offset = *in_ptr & 0x0f;
	      in_ptr++;
	      rep_offset = *in_ptr | (rep_offset << 8);
	      in_ptr++;
	      if (out_ptr-decompressed_data < rep_offset) {
	        return NULL;
	      }

        for ( ; rep_length > 0; rep_length--) {
          *out_ptr = out_ptr[-rep_offset-1];
          out_ptr++;
          if (out_ptr >= out_end) {
            // Need to grow buffer
            decompressed_data = CFRealloc(decompressed_data, unpacked_size*2);
            out_ptr = decompressed_data + unpacked_size;
            unpacked_size *= 2;
            out_end = decompressed_data + unpacked_size;
          }
        }
	    } else {
	      // Just copy byte
        *out_ptr = *in_ptr;
        out_ptr++;
        if (out_ptr >= out_end) {
          // Need to grow buffer
          decompressed_data = CFRealloc(decompressed_data, unpacked_size*2);
          out_ptr = decompressed_data + unpacked_size;
          unpacked_size *= 2;
          out_end = decompressed_data + unpacked_size;
        }
	      in_ptr++;
	    }
	  }
	}

	*decompressed_size = (out_ptr - decompressed_data);
	return decompressed_data;
}

static void do_imet_header(void)
{
	imet_data_t header;

	fread(&header, 1, sizeof header, fp);

	//write_file(&header, sizeof(header), "header.imet"); // we don't need it
}


static int write_imd5_lz77(u8* data, size_t size, char* outname)
{
	imd5_header_t* header = (imd5_header_t*) data;
	u32 tag;
	u32 size_in_imd5;
    u8 md5_calc[16];
	u8 *decompressed_data;
	size_t decompressed_size;

	tag = be32((u8*) &header->imd5_tag);
	if (tag != 0x494D4435) {
	  return -4;
	}

	md5(data+32, size-32, md5_calc);
	if (memcmp(&header->md5, md5_calc, 0x10)) {
		return -5;
	}

	size_in_imd5 = be32((u8*) &header->size);
	if (size_in_imd5 != size - 32) {
	  return -6;
	}

	tag = be32((u8*) &header->payload_tag);
	if (tag == 0x4C5A3737) {
	  // "LZ77" - uncompress
    decompressed_data = decompress_lz77(data + sizeof(imd5_header_t), size - sizeof(imd5_header_t), &decompressed_size);
    if(decompressed_data == NULL)
        return -7;
    write_file(decompressed_data, decompressed_size, outname);
    //printf(", uncompressed %d bytes, md5 ok", decompressed_size);

    CFFree(decompressed_data);
	} else {
	  write_file(&header->payload_tag, size-32, outname);
	  //printf(", md5 ok");
	}
	return 0;
}



int identifiySoundFormat(FILE *fp)
{
 sound_header header;
 u32 tag;
 int ret = -1;
 
 fread(&header, 1, sizeof header, fp);
 tag = be32((u8*) &header.tag);
 
 if      (tag == 0x424E5320 ) // (BNS ) .bns
	{
	 ret = 0;
	}
 else if (tag == 0x52494646 ) // (RIFF) .wav
	{
	 ret = 1;
	}
 else if (tag == 0x464F524D ) // (FORM) .aiff
	{
	 ret = 2;
	}
 return ret;
}


static int do_U8_archive(void)
{
	U8_archive_header header;
	U8_node root_node;
	u32 tag;
	u32 num_nodes;
	U8_node* nodes;
	u8* string_table;
	size_t rest_size;
	unsigned int i;
	u32 data_offset;
	u32 current_offset;
    
	U8_node* node;
	u16 type;
    u16 name_offset;
	u32 my_data_offset;
	u32 size;
	char* name;
	u8* file_data;
	
	
	fread(&header, 1, sizeof header, fp);
	tag = be32((u8*) &header.tag);
	if (tag != 0x55AA382D)
	{
		return -1;
	}

	fread(&root_node, 1, sizeof(root_node), fp);
	num_nodes = be32((u8*) &root_node.size) - 1;
	//printf("Number of files: %d\n", num_nodes);

	nodes = CFMalloc(sizeof(U8_node) * (num_nodes));
	fread(nodes, 1, num_nodes * sizeof(U8_node), fp);

	data_offset = be32((u8*) &header.data_offset);
	rest_size = data_offset - sizeof(header) - (num_nodes+1)*sizeof(U8_node);

	string_table = CFMalloc(rest_size);
	fread(string_table, 1, rest_size, fp);
    current_offset = data_offset;

	for (i = 0; i < num_nodes; i++) 
	{
	    node = &nodes[i];
	    type = be16((u8*)&node->type);
	    name_offset = be16((u8*)&node->name_offset);
	    my_data_offset = be32((u8*)&node->data_offset);
	    size = be32((u8*)&node->size);
	    name = (char*) &string_table[name_offset];

	    if (type == 0x0100)
		{
	      // Directory
		  // do nothing
	    } 
		else
		{
	      // Normal file
	      u8 padding[32];

	      if (type != 0x0000) 
		  {
	         CFFree(nodes);
			 CFFree(string_table);
			 if ( i > 0) CFFree(file_data);
			 return -2;
	      }

	      if (current_offset < my_data_offset) 
		  {
	        int diff = my_data_offset - current_offset;

	        if (diff > 32) 
			{
	         CFFree(nodes);
			 CFFree(string_table);
			 if ( i > 0) CFFree(file_data);
			 return -3;
	        }
	        fread(padding, 1, diff, fp);
	        current_offset += diff;
	      }
		  
		  file_data = CFMalloc(size);
		  fread(file_data, 1, size, fp);
		  
		  if ( i == num_nodes -1 )  // write only the last one
			{
			  int result;
			  result = write_imd5_lz77(file_data, size, name);
			  if(result < 0)
			  {
				CFFree(nodes);
				CFFree(string_table);
				CFFree(file_data);
				return result;
			  }
			} 
	      current_offset += size;
	    }
	}
	CFFree(nodes);
	CFFree(string_table);
	CFFree(file_data);
	return 0;
}

/*
static int do_U8_archive(void)
{
  U8_archive_header header;
  U8_node root_node;
	u32 tag;
	u32 num_nodes;
	U8_node* nodes;
	u8* string_table;
	size_t rest_size;
	unsigned int i;
	u32 data_offset;
	u32 current_offset;
	u16 dir_stack[16];
	int dir_index = 0;

	fread(&header, 1, sizeof header, fp);
	tag = be32((u8*) &header.tag);
	if (tag != 0x55AA382D) {
	  return -1;
	}

	fread(&root_node, 1, sizeof(root_node), fp);
	num_nodes = be32((u8*) &root_node.size) - 1;
	//printf("Number of files: %d\n", num_nodes);

	nodes = malloc(sizeof(U8_node) * (num_nodes));
	fread(nodes, 1, num_nodes * sizeof(U8_node), fp);

	data_offset = be32((u8*) &header.data_offset);
	rest_size = data_offset - sizeof(header) - (num_nodes+1)*sizeof(U8_node);

	string_table = malloc(rest_size);
	fread(string_table, 1, rest_size, fp);
    current_offset = data_offset;

	for (i = 0; i < num_nodes; i++) {
    U8_node* node = &nodes[i];
    u16 type = be16((u8*)&node->type);
    u16 name_offset = be16((u8*)&node->name_offset);
    u32 my_data_offset = be32((u8*)&node->data_offset);
    u32 size = be32((u8*)&node->size);
    char* name = (char*) &string_table[name_offset];
    u8* file_data;

    if (type == 0x0100) {
      // Directory
      mkdir(name, 0777);
      chdir(name);
      dir_stack[++dir_index] = size;
      //printf("%*s%s/\n", dir_index, "", name);
    } else {
      // Normal file
      u8 padding[32];

      if (type != 0x0000) {
         return -2;
      }

      if (current_offset < my_data_offset) {
        int diff = my_data_offset - current_offset;

        if (diff > 32) {
          return -3;
        }
        fread(padding, 1, diff, fp);
        current_offset += diff;
      }

      file_data = malloc(size);
      fread(file_data, 1, size, fp);
      //printf("%*s %s (%d bytes", dir_index, "", name, size);
      int result;
      result = write_imd5_lz77(file_data, size, name);
      if(result < 0)
        return result;
      //printf(")\n");
      current_offset += size;
    }

    while (dir_stack[dir_index] == i+2 && dir_index > 0) {
      chdir("..");
      dir_index--;
    }
	}
	return 0;
}
*/


int RenameSoundFile(const char * filedir)
{
	int sndFormat;
	char newName[255];
	char oldName[255];
	FILE *fp;
	
	sprintf(oldName, "%s/sound.bin", filedir);
	
	fp = fopen(oldName,"rb");

	if(fp!=NULL)
	{
		sndFormat = identifiySoundFormat(fp);
		fclose(fp);
		
		switch (sndFormat)
		{
			case 0:
				sprintf(newName, "%s/sound.bns", filedir);
				break;
			case 1:
				sprintf(newName, "%s/sound.wav", filedir);
				break;
			case 2:
			    sprintf(newName, "%s/sound.aiff", filedir);
				break;
			default:
				return -1;
				break;
		}
		rename(oldName, newName);
	}
	return 0;
}


int extractBanner(const char * filepath, const char * destpath)
{
    int ret;

	fp = fopen(filepath, "rb");
    if(fp == NULL)
		return -1;

    mkdir(destpath, 0777);
    chdir(destpath);

    do_imet_header();
    ret = do_U8_archive();

	fclose(fp);

	return ret;
}
