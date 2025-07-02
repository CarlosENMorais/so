/* Criar um sistema de arquivos de somente leitura
	-> Armazenamento em um sipositivo de blocos
		* Tamanho do block pode ser fixo (e.g. 512 bytes)
	-> Arquivos serão somente lidos
	-> Busca por arquivos eficiente
	Qual seria um formato adequado?
*/
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "header.h"

#define blk_size 512
#define n_max_files 60
#define nr_blocks 64

// Metadados sobre o Disco
struct super{
	uint32_t sign;
	uint32_t block_size;
	uint64_t nr_files;
}__attribute__((packed));

// Estrutura do Elementos do Indice [ 32 bytes ]
struct file{
	char name[20];
	uint32_t index;  //4 bytes
	uint64_t size; //8 bytes
}__attribute__((packed));

struct block{
	uint8_t byte[512];
};

// Indice dos arquivos no disco
struct inode{
	struct file files[32]; // Limita quantidade de arquivos no disco
};

// Estrutura do Disco final
struct disk{
	struct super super;
	struct inode inode;
	struct block block[64];
};

struct disk Disk;

int fs_open(const char* pathname, int flags){
	for(int i = 0; i < 60; i++){
		if(strcmp(pathname, Disk.inode.files[i].name) == 0){
			return Disk.inode.files[i].index;
		}
	}
	return -1;
}

ssize_t fs_read(int fd, void* buffer, size_t count)
{
	uint64_t block_index = Disk.inode.files[fd].index;
	uint64_t file_size = Disk.inode.files[fd].size;

	ssize_t to_read = (count < file_size) ? count : file_size;

	uint8_t* b = buffer;
	uint8_t* s = Disk.block[block_index].byte;
	
	for(size_t i = to_read; i>0; --i){
		*b++ = *s++;
	}

	return to_read;
}


int main(){
	return 0;
}

