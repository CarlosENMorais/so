#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/stat.h>
#include <sys/mman.h>

struct ext2_super_block{
    uint32_t s_inodes_count;        // Total de inodes no sistema
    uint32_t s_blocks_count;        // Total de blocos no sistema
    uint32_t s_r_blocks_count;      // Blocos reservados para o superusuário
    uint32_t s_free_blocks_count;   // Blocos livres
    uint32_t s_free_inodes_count;   // Inodes livres
    uint32_t s_first_data_block;    // Primeiro bloco de dados
    uint32_t s_log_block_size;      // Tamanho do bloco (log2(block_size) - 10)
    uint32_t s_log_frag_size;       // Tamanho do fragmento (log2(frag_size) - 10)
    uint32_t s_blocks_per_group;    // Blocos por grupo
    uint32_t s_frags_per_group;     // Fragmentos por grupo
    uint32_t s_inodes_per_group;    // Inodes por grupo
    uint32_t s_mtime;               // Tempo da última montagem
    uint32_t s_wtime;               // Tempo da última escrita
    uint16_t s_mnt_count;           // Contador de montagens
    uint16_t s_max_mnt_count;       // Máximo de montagens antes de fsck
    uint16_t s_magic;               // Assinatura mágica (0xEF53)
    uint16_t s_state;               // Estado do sistema de arquivos
    uint16_t s_errors;              // Comportamento quando erros são detectados
    uint16_t s_minor_rev_level;     // Nível de revisão menor
    uint32_t s_lastcheck;           // Tempo da última verificação
    uint32_t s_checkinterval;       // Intervalo máximo entre verificações
    uint32_t s_creator_os;          // OS que criou o sistema
    uint32_t s_rev_level;           // Nível de revisão
    uint16_t s_def_resuid;          // UID padrão para blocos reservados
    uint16_t s_def_resgid;          // GID padrão para blocos reservados
    
    // Campos para EXT2_DYNAMIC_REV (revisão >= 1)
    uint32_t s_first_ino;           // Primeiro inode não reservado
    uint16_t s_inode_size;          // Tamanho da estrutura inode
    uint16_t s_block_group_nr;      // Número do grupo de blocos deste superblock
    uint32_t s_feature_compat;      // Features compatíveis
    uint32_t s_feature_incompat;    // Features incompatíveis
    uint32_t s_feature_ro_compat;   // Features somente leitura compatíveis
    uint8_t  s_uuid[16];            // UUID do volume (128 bits)
    char     s_volume_name[16];     // Nome do volume
    char     s_last_mounted[64];    // Diretório onde foi montado pela última vez
    uint32_t s_algorithm_usage_bitmap; // Para compressão
    
    // Performance hints
    uint8_t  s_prealloc_blocks;     // Blocos para pré-alocar para arquivos
    uint8_t  s_prealloc_dir_blocks; // Blocos para pré-alocar para diretórios
    uint16_t s_reserved_gdt_blocks; // Blocos reservados para expansão GDT
    
    // Journaling support (EXT3)
    uint8_t  s_journal_uuid[16];    // UUID do journal
    uint32_t s_journal_inum;        // Número do inode do journal
    uint32_t s_journal_dev;         // Dispositivo do journal
    uint32_t s_last_orphan;         // Início da lista de inodes órfãos
    
    // Directory indexing support
    uint32_t s_hash_seed[4];        // Seeds para hash do diretório
    uint8_t  s_def_hash_version;    // Versão padrão do hash
    uint8_t  s_jnl_backup_type;     // Tipo de backup do journal
    uint16_t s_desc_size;           // Tamanho do descritor de grupo
    
    uint32_t s_default_mount_opts;  // Opções padrão de montagem
    uint32_t s_first_meta_bg;       // Primeiro bloco de metadados
    uint32_t s_mkfs_time;           // Tempo de criação do sistema
    uint32_t s_jnl_blocks[17];      // Backup dos blocos do journal
    
    // Campos para volumes de 64 bits (EXT4)
    uint32_t s_blocks_count_hi;     // Bits altos do contador de blocos
    uint32_t s_r_blocks_count_hi;   // Bits altos dos blocos reservados
    uint32_t s_free_blocks_count_hi; // Bits altos dos blocos livres
    uint16_t s_min_extra_isize;     // Tamanho extra mínimo do inode
    uint16_t s_want_extra_isize;    // Tamanho extra desejado do inode
    uint32_t s_flags;               // Flags diversas
    uint16_t s_raid_stride;         // Stride do RAID
    uint16_t s_mmp_update_interval; // Intervalo de atualização MMP
    uint64_t s_mmp_block;           // Bloco para proteção multi-mount
    uint32_t s_raid_stripe_width;   // Largura da faixa RAID
    uint8_t  s_log_groups_per_flex; // Grupos por flex group
    uint8_t  s_checksum_type;       // Tipo de checksum dos metadados
    uint16_t s_reserved_pad;        // Padding
    uint64_t s_kbytes_written;      // Kbytes escritos no tempo de vida
    uint32_t s_snapshot_inum;       // Número do inode do snapshot
    uint32_t s_snapshot_id;         // ID sequencial do snapshot
    uint64_t s_snapshot_r_blocks_count; // Blocos reservados para snapshot
    uint32_t s_snapshot_list;       // Inode do cabeçalho da lista de snapshots
    uint32_t s_error_count;         // Número de erros do sistema
    uint32_t s_first_error_time;    // Tempo do primeiro erro
    uint32_t s_first_error_ino;     // Inode envolvido no primeiro erro
    uint64_t s_first_error_block;   // Bloco envolvido no primeiro erro
    uint8_t  s_first_error_func[32]; // Função onde ocorreu o primeiro erro
    uint32_t s_first_error_line;    // Linha onde ocorreu o primeiro erro
    uint32_t s_last_error_time;     // Tempo do erro mais recente
    uint32_t s_last_error_ino;      // Inode envolvido no erro mais recente
    uint32_t s_last_error_line;     // Linha do erro mais recente
    uint64_t s_last_error_block;    // Bloco envolvido no erro mais recente
    uint8_t  s_last_error_func[32]; // Função do erro mais recente
    uint8_t  s_mount_opts[64];      // Opções de montagem padrão
    uint32_t s_usr_quota_inum;      // Inode para cota de usuário
    uint32_t s_grp_quota_inum;      // Inode para cota de grupo
    uint32_t s_overhead_clusters;   // Overhead em clusters
    uint32_t s_backup_bgs[2];       // Grupos com backup de superblock
    uint8_t  s_encrypt_algos[4];    // Algoritmos de criptografia
    uint8_t  s_encrypt_pw_salt[16]; // Salt para derivação de senha
    uint32_t s_lpf_ino;             // Inode do diretório lost+found
    uint32_t s_prj_quota_inum;      // Inode para cota de projeto
    uint32_t s_checksum_seed;       // Seed para checksum
    uint32_t s_reserved[98];        // Preenchimento para 1024 bytes
    uint32_t s_checksum;            // Checksum do superblock
};

struct ext2_group_desc {
    uint32_t bg_block_bitmap;      // Número do bloco do bitmap de blocos
    uint32_t bg_inode_bitmap;      // Número do bloco do bitmap de inodes
    uint32_t bg_inode_table;       // Número do bloco da tabela de inodes
    uint16_t bg_free_blocks_count; // Quantos blocos livres há nesse grupo
    uint16_t bg_free_inodes_count; // Quantos inodes livres há nesse grupo
    uint16_t bg_used_dirs_count;   // Quantos diretórios existem nesse grupo
    uint16_t bg_pad;               // Padding (alinhamento)
    uint32_t bg_reserved[3];       // Reservado para uso futuro
};

struct ext2_inode {
    uint16_t i_mode;        // Offset 0, Size 2 bytes
    uint16_t i_uid;         // Offset 2, Size 2 bytes
    uint32_t i_size;        // Offset 4, Size 4 bytes
    uint32_t i_atime;       // Offset 8, Size 4 bytes
    uint32_t i_ctime;       // Offset 12, Size 4 bytes
    uint32_t i_mtime;       // Offset 16, Size 4 bytes
    uint32_t i_dtime;       // Offset 20, Size 4 bytes
    uint16_t i_gid;         // Offset 24, Size 2 bytes
    uint16_t i_links_count; // Offset 26, Size 2 bytes
    uint32_t i_blocks;      // Offset 28, Size 4 bytes
    uint32_t i_flags;       // Offset 32, Size 4 bytes
    uint32_t i_osd1;        // Offset 36, Size 4 bytes
    uint32_t i_block[15];   // Offset 40, Size 15 x 4 bytes = 60 bytes
    uint32_t i_generation;  // Offset 100, Size 4 bytes
    uint32_t i_file_acl;    // Offset 104, Size 4 bytes
    uint32_t i_dir_acl;     // Offset 108, Size 4 bytes
    uint32_t i_faddr;       // Offset 112, Size 4 bytes
    uint8_t  i_osd2[12];    // Offset 116, Size 12 bytes
};

// Escreve o conteúdo do inode em um arquivo, até 12*Block_Size Bytes
void retrieve(uint32_t index, struct ext2_inode* piNode, uint32_t s_log_block_size, void* map){
    // Número de Blocos de memória alocados pelo arquivo.
    int nBlocks = piNode->i_blocks/(2<<s_log_block_size);
    if(nBlocks > 12){
        printf("Arquivo muito grande. Implementar função adequada.");
    }

    int bs = 1024 << s_log_block_size;


    
    char name[32];
    sprintf(name, "./recovered/%d", index);
    mkdir("./recovered", 0777);
    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if(fd == -1){
        printf("Erro ao criar o arquivo: %s", name);
        return;
    }
    
    printf("######## DEGUB DO iNODE ##########\n");
    printf("I_SIZE:\t%u\n", piNode->i_size);
    printf("nBOLOCKS:\t%u\n", nBlocks);
    printf("delTime:\t%u\n", piNode->i_dtime);
    printf("bLinks:\t%u\n", piNode->i_links_count);
    printf("==================================\n");


    int escrito = 0;
    for(int i = 0; i < 12 && piNode->i_size > escrito; i++){
        // Verifica validade do identificador do bloco;
        // if(piNode->i_block[i] == 0){
        //     continue;
        // }
        // Endereço do bloco a ser escrito
        void* block = (uint8_t*)map + (piNode->i_block[i] * bs);
        if((piNode->i_size - escrito) > bs){
            escrito = escrito + write(fd, block, bs);
        }else{
            escrito = escrito + write(fd, block, piNode->i_size - escrito);
        }
    }            
    close(fd);

}

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "Uso: %s <caminho_para_imagem>\n", argv[0]);
        return 1;
    }

    const char* caminho = argv[1];
    int fd = open(caminho, O_RDONLY);
    if(fd == -1){
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    printf("Arquivo '%s' aberto com sucesso! (fd = %d)\n", caminho, fd);

    struct stat st;
    if(fstat(fd, &st) < 0){
        perror("Erro ao obter tamanho do arquivo");
        close(fd);
        return 1;
    }

	printf("Modo: %u\n",st.st_mode);
	printf("UID: %u\n",st.st_uid);

    // Mapeia o arquivo na memória (leitura apenas)
    void* map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(map == MAP_FAILED){
        perror("Erro no mmap");
        close(fd);
        return 1;
    }

    // Bloco Zero
	void* blk0 = (uint8_t*)map + 1024;

	// Super Bloco
    struct ext2_super_block* sb = (struct ext2_super_block*)blk0;

	// Block Size
	uint32_t bs = 1024 << sb->s_log_block_size;

	// Block Group
	struct ext2_group_desc* bg = (struct ext2_group_desc*)((uint8_t*)blk0 + bs);
	
	// Inode Table
    struct ext2_inode* it = (struct ext2_inode*)((uint8_t*)blk0 + bg->bg_inode_table * bs);
    
    // sb->s_inodes_per_group
    // sb->s_inode_size
    
    printf("Endereço da Tabela de iNodes: %p\nTamanho da Tabela: %d\nTamanho do iNode: %d\n", 
        it, sb->s_inodes_per_group, sb->s_inode_size);
    
    printf("Escaneando Tabela de iNodes\n");
    for(int i = 0; i < sb->s_inodes_per_group && i < 42; i++){
        void* piNode = (uint8_t*)it + sb->s_inode_size*i;
        struct ext2_inode* iNode = (struct ext2_inode*)((uint8_t*)it + sb->s_inode_size*i);


        if(iNode->i_mode >> 12 == 8){
            printf("%p\n",piNode);
            printf("%p\n",iNode);
            printf("Analizando o iNode [%i] i_mode: 0x%04X Endereço: %p Offset: %i\n",
            i, iNode->i_mode, piNode, sb->s_inode_size*i);
            // printf("[ AVISO ] - Copiando conteúdo de um iNode para um arquivo\n");
            retrieve(i, piNode, sb->s_log_block_size, map);
        }
        
    }
	/* Aqui eu tenho que testar cada inode
	Se o inode for de um arquivo e tiver sido deletado
	acessar os bloco de dados correspondentes e montar esse arquivo 
	em um outro lugar.
	*/

    printf("Encerrando programa!\n");
    // Desmapeia a memória e fecha o arquivo
    if(munmap(map, st.st_size) < 0){
        perror("Erro no munmap");
    }

    close(fd);
	return 0;
}
