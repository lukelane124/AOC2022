#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "AOC_utils.h"

char buf[1024];

enum FS_e
{
    FOLDERT,
    FILET
};
typedef enum FS_e FS_e;


struct FS_t
{
    FS_e t;
    char* name;
    struct Folder* parent;
    unsigned int size;
};
typedef struct FS_t FS_t;

struct Folder
{
    FS_t t;
    FS_t** folder;
    unsigned int nItems;
    unsigned int nAlloc;
    int visited;
    int index;
};
typedef struct Folder Folder;

struct File
{
    FS_t t;
    // unsigned int size;
    // could remove this entirely...
};
typedef struct File File;

File* File_create(char* fileName, int size)
{
    File* f = malloc(sizeof(File));
    if (f)
    {
        memset(f, 0, sizeof(File));
        f->t.name = strDupe(fileName);
        f->t.t = FILET;
        f->t.size = size;
    }
    return f;
}

void Folder_add(Folder* F, FS_t* o)
{
    if (F->nItems >= F->nAlloc)
    {
        F->nAlloc += 10;
        F->folder = realloc(F->folder, (F->nAlloc * sizeof(FS_t*)));
        o->parent = F;
    }

    F->folder[F->nItems++] = o;
}

Folder* Folder_create(char* folderName)
{
    fprintf(stderr, "Creating folder: %s\n", folderName);
    Folder* f = malloc(sizeof(Folder));
    if (f)
    {
        memset(f, 0, sizeof(Folder));
        f->t.name = strDupe(folderName);
        f->t.t = FOLDERT;
        f->t.parent = NULL;
        f->t.size = 0;
        f->index = 0;
    }
    return f;
}

// Folder* root = NULL;
Folder* currentFolder = NULL;

void File_print(File* file)
{
    fprintf(stderr, "%s\n", file->t.name);
}

void Folder_print(Folder* folder)
{
    fprintf(stderr, "dir %s %d\n", folder->t.name, folder->t.size);
    for (int i = 0; i < folder->nItems; i++)
    {
        FS_t* F = folder->folder[i];
        if (F->t == FOLDERT)
        {
            Folder_print((Folder*) F);
        }
        else
        {
            File* file = (File*) F;
            fprintf(stderr, "%d %s\n", file->t.size, file->t.name);
        }
    }
}

unsigned int _calculateSize(FS_t* F)
{
    unsigned int ret = 0;
    switch (F->t)
    {
        case FOLDERT:
        {
            Folder* folder = (Folder*) F;
            if (folder->visited)
            {
                return folder->t.size;
            }
            for (int i = 0; i < folder->nItems; i++)
            {
                ret += _calculateSize((FS_t*) folder->folder[i]);
            }
            folder->visited = 1;
            folder->t.size = ret;
            return ret;
            break;
        }
        case FILET:
        default:
        {
            return F->size;
            break;
        }
    }
}

void parser_add_line(char* line, FILE* F)
{
    char* clean = trim(line);
    fprintf(stderr, "Parsing: %s\n", clean);
    if (clean[0] == '$')
    {
        // command.
        fprintf(stderr, "Command\n");
    
        if (clean[2] == 'l')
        {
            // LS
            // Following lines are directory entries.
        }
        else if (clean[2] == 'c')
        {
            clean += 5;
            fprintf(stderr, "folder_name[%lu]: %s\n", strlen(clean), clean);
            if (!strcmp("..", clean))
            {
                fprintf(stderr, "moving up.\n");
                if (currentFolder != NULL &&  currentFolder->t.parent != NULL)
                {
                    fprintf(stderr, "Leaving directory: %s --> %s\n", currentFolder->t.name, currentFolder->t.parent->t.name);
                    unsigned int size = _calculateSize((FS_t*) currentFolder);
                    currentFolder = currentFolder->t.parent;
                }
                else
                {
                    fprintf(stderr, "Encountered null parent.\n");
                    fprintf(stderr, "currentFolder: %p; foldername: %s\n", currentFolder, (currentFolder ? currentFolder->t.name : "NULL"));
                    fprintf(stderr, "currentFolder->t.parent: %p; p_foldername: %s\n", currentFolder->t.parent, (currentFolder->t.parent ? currentFolder->t.parent->t.name : "NULL"));
                    exit(-2);
                }
            }
            else
            {
                Folder* f = Folder_create(clean);
                if (currentFolder != NULL)
                {
                    Folder_add(currentFolder, (FS_t*) f);
                    f->t.parent = currentFolder;
                }
                currentFolder = f;
            }
        }
    }
    else // line[0] != '$'
    {
        // line from directory listing.
        // fprintf(stderr, "directory listing, create file system object?.\n");
        if (clean[0] != 'd')
        {
            int size;
            sscanf(clean, "%d %1024s", &size, buf);
            // fprintf(stderr, "%d %s\n", size, buf);
            FS_t* f = (FS_t*) File_create(buf, size);
            Folder_add(currentFolder, f);
        }
        else
        {
            // Folder. Planning to ignore until we CD into it.
            // fprintf(stderr, "%s; not adding to list until we `cd` into %s\n", clean, clean);
        }
    }
    
}

// unsigned int _special_calculate_contained_size(Folder* root)
// {

// }

unsigned int _special_calculate_running_size(Folder* root)
{
    unsigned int runningFolderSize = 0;
    Folder* currentFolder = root;
    while(1)
    {
        while(currentFolder->index < currentFolder->nItems)
        {
            FS_t* O = currentFolder->folder[currentFolder->index++];
            if (O->t == FOLDERT)
            {
                fprintf(stderr, "--> %s\n", O->name);
                currentFolder = (Folder*) O;
            }
        }
        
        // For the current folder we've exhausted all sub elements, no deeper folders.
        if (currentFolder->t.size < 100000)
        {
            runningFolderSize += currentFolder->t.size;
        }

        if (!currentFolder->t.parent)
        {
            // Root directory.
            break;
        }
        else
        {
            currentFolder = currentFolder->t.parent;
        }
    }
    return runningFolderSize;
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = 0;
    int status = 0;
    char* filename = NULL;
    char dayNum = 0;
    static int map[128] = {0};

    if (argc < 2)
    {
        AOC_Usage(argv[0], "Not enough arguments");
    }
    
    fIn = fopen(argv[1], "r");

    if (fIn == NULL)
    {
        fprintf(stderr, "Unable to open file: %s\n%s\n", argv[1], strerror(errno));
        exit(-3);
    }
    else
    {
        char* line = NULL;
        size_t size = 0;
        ssize_t nChars = 0;

        // root = Folder_create("/");

        while(!feof(fIn))
        {
           nChars = getline(&line, &size, fIn);
           parser_add_line(line, fIn);
        }

        // File structure is parsed.
        
        // Find the top level folder
        Folder* root = currentFolder;
        while(root->t.parent) root = root->t.parent;

        root->t.size = _calculateSize((FS_t*) root);

        Folder_print(root);

        unsigned int running = _special_calculate_running_size(root);
        printf("Running: %u\n", running);

        free(line);
        fclose(fIn);
        
        fIn = NULL;
    }

    return 0;
}