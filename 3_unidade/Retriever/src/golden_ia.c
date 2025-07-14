#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <caminho_para_imagem>\n", argv[0]);
        return 1;
    }

    const char* pathfile = argv[1];
    int fd = open(pathfile, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    printf("Image '%s' opened with success on (fd = %d)\n", pathfile, fd);

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("Erro ao obter tamanho do arquivo");
        close(fd);
        return 1;
    }

    // if (st.st_size < 1024 + sizeof(struct ext2_super_block)) {
    //     fprintf(stderr, "Imagem muito pequena para ser EXT2\n");
    //     close(fd);
    //     return 1;
    // }

    void* map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("Erro no mmap");
        close(fd);
        return 1;
    }
    printf("Virtual address: %p\nFile Size: %llu Bytes\n\n", map, st.st_size);

    
    printf("Accessing superblock struct:\n");
    // Super Block
    uint8_t* sb = ((uint8_t*)map + 1024);

    // Magic Number of file system. For ext2 is 0xEF53.
    uint16_t s_magic = *(uint16_t*)(sb+56);
    if (s_magic != 0xEF53) {
        fprintf(stderr, "Não é um sistema de arquivos EXT2 válido (magic: 0x%x)\n", s_magic);
        munmap(map, st.st_size);
        close(fd);
        return 1;
    }

    uint32_t s_log_block_size = *(uint32_t*)(sb+24);
    // Block Size.
    uint64_t bs = 1024 << s_log_block_size;
    printf(" - block_size:\t%llu Bytes\n",bs);
    if (bs < 1024 || bs > 65536) {
        fprintf(stderr, "Tamanho de bloco inválido: %u\n", bs);
        munmap(map, st.st_size);
        close(fd);
        return 1;
    }

    uint32_t s_blocks_count = *(uint32_t*)(sb+4);    
    uint32_t s_blocks_per_group = *(uint32_t*)(sb+32);    
    uint32_t num_groups = (s_blocks_count + s_blocks_per_group - 1) / s_blocks_per_group;
    uint32_t s_inodes_count = *(uint32_t*)(sb);
    uint32_t s_inodes_per_group = *(uint32_t*)(sb+40);
    uint32_t s_first_ino = *(uint32_t*)(sb+84);

    printf("=== Informações do Sistema ===\n");

    printf(" - s_magic:\t0x%04X\n", s_magic);
    printf(" - Block size: %u bytes\n", bs);
    printf(" - Total de inodes: %u\n", s_inodes_count);
    printf(" - Inodes por grupo: %u\n", s_inodes_per_group);
    printf(" - Número de grupos: %u\n", num_groups);
    printf(" - Primeiro inode não reservado: %u\n", s_first_ino);

    printf("================================\n\n");

    uint32_t gd_offset = (bs == 1024) ? 2048 : bs;
    struct gd_table = (struct ext2_group_desc*)((uint8_t*)map + gd_offset);

    if (mkdir("recovered", 0755) == -1 && errno != EEXIST) {
        perror("Erro ao criar diretório 'recovered'");
        munmap(map, st.st_size);
        close(fd);
        return 1;
    }

    printf("Procurando arquivos deletados...\n");
    uint32_t files_recovered = 0;

    for (uint32_t group = 0; group < num_groups; group++) {
        struct ext2_group_desc* gd = &gd_table[group];
        printf("\n--- Processando grupo %u ---\n", group);
        printf("Tabela de inodes no bloco: %u\n", gd->bg_inode_table);
        printf("Inodes livres: %u\n", gd->bg_free_inodes_count);
        struct ext2_inode* it = (struct ext2_inode*)((uint8_t*)map + gd->bg_inode_table * bs);

        for (uint32_t local_inode = 0; local_inode < sb->s_inodes_per_group; local_inode++) {
            uint32_t inode_num = group * sb->s_inodes_per_group + local_inode + 1;
            if (inode_num < sb->s_first_ino && inode_num != 2) {
                continue;
            }

            struct ext2_inode* inode = &it[local_inode];
            uint16_t type = inode->i_mode >> 12;

            if (type == 8 && inode->i_size > 0 && inode->i_dtime == 0) {
                printf("Arquivo encontrado! SIZE: %u\tINODEG: %u\tINODEL: %u\n",
                       inode->i_size, inode_num, local_inode);
            }

            if (type == 8 && inode->i_dtime != 0 && inode->i_size > 0) {
                if (!is_inode_used(map, gd, local_inode, bs)) {
                    printf("Encontrado arquivo deletado:\n");
                    printf("  Inode: %u (grupo %u, local %u)\n", inode_num, group, local_inode);
                    printf("  Tamanho: %u bytes\n", inode->i_size);
                    printf("  Deletado em: %s", ctime((time_t*)&inode->i_dtime));
                    printf("  Blocos: %u\n", inode->i_blocks);

                    int has_valid_blocks = 0;
                    for (int i = 0; i < 12; i++) {
                        if (inode->i_block[i] != 0) {
                            has_valid_blocks = 1;
                            break;
                        }
                    }

                    if (has_valid_blocks) {
                        char filename[256];
                        snprintf(filename, sizeof(filename), "recovered/file_%u_group_%u.recovered",
                                inode_num, group);
                        int out_fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                        if (out_fd < 0) {
                            perror("Erro ao criar arquivo recuperado");
                            continue;
                        }

                        recover_complete_file(out_fd, inode, map, bs, total_blocks);
                        close(out_fd);
                        files_recovered++;
                        printf("  Arquivo salvo como: %s\n", filename);
                    } else {
                        printf("  Arquivo sem blocos válidos - pulando\n");
                    }
                }
            }
        }
    }

    printf("\n=== Resumo ===\n");
    printf("Recuperação concluída! %u arquivos recuperados.\n", files_recovered);
    printf("Verifique a pasta 'recovered' para os arquivos recuperados.\n");

    munmap(map, st.st_size);
    close(fd);
    return 0;
}
