#include "inode.h"


/* Create an empty disk */
int create_disk(char* disk_name)
{
    char disk_buffer[BLOCK_SIZE]; // buffer for disk 
    if (access(disk_name, F_OK) != -1) // access disk succeeds
    {
        //cout << string(RED) << "Virtual Disk already exists !!!" << string(DEFAULT) << endl;
        return -1;
    }
    diskptr = fopen(disk_name, "wb");
    memset(disk_buffer, 0, BLOCK_SIZE); // intialize disk buffer null

    /* Initialize every block on disk as null  */
    for (int i = 0; i < DISK_BLOCKS; ++i)
        fwrite(disk_buffer, 1, BLOCK_SIZE, diskptr);

    struct super_block sb;

    /* Set superblock, file-inode mapping, and inode blocks as used */
    for (int i = 0; i < sb.starting_index_of_data_blocks; ++i)
        sb.datablock_freelist[i] = false; // set false meaning in use

    /* Set data blocks as free */
    for (int i = sb.starting_index_of_data_blocks; i < DISK_BLOCKS; ++i)
        sb.datablock_freelist[i] = true; // set true meaning free

    /* Set inodes as free */
    for (int i = 0; i < NO_OF_INODES; ++i)
        sb.inode_freelist[i] = false;
    /* Initialize inode pointers */
    for (int i = 0; i < NO_OF_INODES; ++i) {
        for (int j = 0; j < 12; j++) {
            inode_arr[i].pointer[j] = -1;
        }
    }

    /* Store superblock first */
    int len_sb = sizeof(struct super_block);
    char sb_buffer[len_sb];
    fseek(diskptr, 0, SEEK_SET);
    memset(sb_buffer, 0, len_sb);
    memcpy(sb_buffer, &sb, sizeof(sb));
    fwrite(sb_buffer, sizeof(char), sizeof(sb), diskptr);

    /* Store file-inode mapping second */
    fseek(diskptr, (sb.no_of_blocks_used_by_superblock) * BLOCK_SIZE, SEEK_SET);
    int len_map = sizeof(file_inode_mapping_arr);
    char map_buffer[len_map];
    memset(map_buffer, 0, len_map);
    memcpy(map_buffer, file_inode_mapping_arr, len_map);
    fwrite(map_buffer, sizeof(char), len_map, diskptr);

    /* Store inodes third */
    fseek(diskptr, (sb.starting_index_of_inodes) * BLOCK_SIZE, SEEK_SET);
    int len_inode = sizeof(inode_arr);
    char inode_buffer[len_inode];
    memset(inode_buffer, 0, len_inode);
    memcpy(inode_buffer, inode_arr, len_inode);
    fwrite(inode_buffer, sizeof(char), len_inode, diskptr);

    fclose(diskptr);
    return 1;
}