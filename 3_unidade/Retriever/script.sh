#!/bin/bash

# Script to create an EXT2 filesystem image for testing golden_ia.c

set -e  # Exit on any error

# Configuration
IMAGE_FILE="ext2.img"
IMAGE_SIZE_MB=1
MOUNT_POINT="/mnt/test_ext2"
TEST_FILES=("file1.txt" "file2.txt" "file3.txt")
RECOVERED_DIR="recovered"

# Check if running as root (needed for mount and mkfs)
if [ "$EUID" -ne 0 ]; then
    echo "Este script deve ser executado como root (use sudo)"
    exit 1
fi

# Check if required commands are available
for cmd in dd mkfs.ext2 mount umount; do
    if ! command -v $cmd &> /dev/null; then
        echo "Erro: comando '$cmd' não encontrado. Instale os utilitários necessários."
        exit 1
    fi
done

# Create the image file
echo "Criando imagem de $IMAGE_SIZE_MB MB: $IMAGE_FILE..."
dd if=/dev/zero of="$IMAGE_FILE" bs=1M count="$IMAGE_SIZE_MB" status=progress

# Format as EXT2
echo "Formatando imagem como EXT2..."
mkfs.ext2 -F "$IMAGE_FILE"

# Create mount point
if [ ! -d "$MOUNT_POINT" ]; then
    echo "Criando ponto de montagem: $MOUNT_POINT..."
    mkdir -p "$MOUNT_POINT"
fi

# Mount the image
echo "Montando imagem em $MOUNT_POINT..."
mount -o loop "$IMAGE_FILE" "$MOUNT_POINT"

# Create test files
echo "Criando arquivos de teste..."
for file in "${TEST_FILES[@]}"; do
    echo "Conteúdo de teste para $file" > "$MOUNT_POINT/$file"
    ls -l "$MOUNT_POINT/$file"
done

# Delete one file to simulate a deleted file
echo "Deletando ${TEST_FILES[0]} para teste de recuperação..."
rm "$MOUNT_POINT/${TEST_FILES[0]}"

# Sync and unmount
sync
echo "Desmontando imagem..."
umount "$MOUNT_POINT"

# Clean up mount point
rmdir "$MOUNT_POINT"

# Ensure the recovered directory exists
if [ ! -d "$RECOVERED_DIR" ]; then
    echo "Criando diretório para arquivos recuperados: $RECOVERED_DIR..."
    mkdir "$RECOVERED_DIR"
    chmod 777 "$RECOVERED_DIR"
fi

echo "Imagem $IMAGE_FILE criada com sucesso!"
echo "Arquivos criados: ${TEST_FILES[*]}"
echo "Arquivo deletado: ${TEST_FILES[0]}"
echo "Para testar o programa, compile golden_ia.c e execute:"
echo "  gcc -o recover golden_ia.c"
echo "  ./recover $IMAGE_FILE"
echo "Os arquivos recuperados serão salvos em '$RECOVERED_DIR'."