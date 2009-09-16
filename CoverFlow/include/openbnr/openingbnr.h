#ifndef _OPENINGBNR_H_
#define _OPENINGBNR_H_
int extractBanner(const char * filepath, const char * destpath);
u8* decompress_lz77(u8 *data, size_t data_size, size_t* decompressed_size);
int RenameSoundFile(const char * filename);
int identifiySoundFormat(FILE * fp);
u16 be16(const u8 *p);
u32 be32(const u8 *p);

#endif
