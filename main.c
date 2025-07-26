#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int bufferSize = 100;

// -------------------- File Hash --------------------
int fileHash(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        printf("Error opening file: %s\n", fileName);
        return -1;
    }

    char fileData[100];
    int bytesRead;
    int fileHash = 0;

    while ((bytesRead = fread(fileData, sizeof(char), bufferSize, file))) {
        for (int i = 0; i < bytesRead; i++) {
            fileHash = (fileHash * 31 + fileData[i]) % 1000;
        }
    }

    fclose(file);
    return fileHash;
}

// -------------------- Create Blob --------------------
int createBlob(char *fileName) {
    int hash = fileHash(fileName);
    if (hash == -1) return -1;

    char fileNameWithHash[100];
    sprintf(fileNameWithHash, ".vcs/blobs/%d.txt", hash);
    FILE * fileCheck = fopen(fileNameWithHash, "r");
    if(!fileCheck){
    
    FILE *blobFile = fopen(fileNameWithHash, "w");
    if (!blobFile) {
        printf("Error opening blob file\n");
        return -1;
    }

    FILE *file = fopen(fileName, "r");
    if (!file) {
        printf("Error opening file: %s\n", fileName);
        fclose(blobFile);
        return -1;
    }

    char fileData[bufferSize];
    int bytesRead;
    while ((bytesRead = fread(fileData, sizeof(char), bufferSize, file))) {
        fwrite(fileData, sizeof(char), bytesRead, blobFile);
    }

    fclose(file);
    fclose(blobFile);
    printf("Blob created: %s\n", fileNameWithHash);
    }
    return hash;
}

// -------------------- Stage Structure --------------------
typedef struct stage {
    char fileName[100];
} stage;

// -------------------- Init Repo --------------------
void initRepo() {
    system("mkdir -p .vcs/commits");
    system("mkdir -p .vcs/blobs");
    FILE *headFile = fopen(".vcs/HEAD.txt", "w");
    if (!headFile) {
        printf("Error creating HEAD file.\n");
        return;
    }
    FILE *indexFile = fopen(".vcs/index", "w");
    if (!indexFile) {
        printf("Error creating index file.\n");
        fclose(headFile);
        return;
    }        

}

// -------------------- Stage File --------------------
int stageFile(stage *stageArea, int *stageCount, const char *fileName) {
    strcpy(stageArea[*stageCount].fileName, fileName);
    (*stageCount)++;
    return 0;
}

// -------------------- Save Index --------------------
void saveIndex(stage *stageArea, int stageCount) {
    FILE *indexFile = fopen(".vcs/index", "w");
    for (int i = 0; i < stageCount; i++) {
        fprintf(indexFile, "%s\n", stageArea[i].fileName);
    }
    fclose(indexFile);
}

// -------------------- Commit Functions --------------------
int getNextCommitId() {
    FILE *f = fopen(".vcs/HEAD.txt", "r");
    int id = 0;
    if (f) {
        fscanf(f, "%d", &id);
        fclose(f);
    }
    return id;

}

void updateHead(int newId) {
    FILE *f = fopen(".vcs/HEAD.txt", "w");
    fprintf(f, "%d", newId);
    fclose(f);
}

void storeCommit(stage *stageArea, int stageCount, int commitId) {
    char commitFileName[100];
    sprintf(commitFileName, ".vcs/commits/commit_%d.txt", commitId);

    FILE *commitFile = fopen(commitFileName, "a");
    if (!commitFile) {
        printf("Failed to create commit file.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", t);

    fprintf(commitFile, "Commit ID: %d\n", commitId);
    fprintf(commitFile, "Timestamp: %s\n", timeStr);
    fprintf(commitFile, "Files:\n");

    for (int i = 0; i < stageCount; i++) {
        int hash = createBlob(stageArea[i].fileName);
        if (hash != -1) {
            fprintf(commitFile, "%s -> %d\n", stageArea[i].fileName, hash);
        }
    }

    fclose(commitFile);
    updateHead(commitId);

    printf("✅ Commit %d created successfully.\n", commitId);
}


void loadCommit(int commitId){
  char fileName[100]=".vcs/commits/commit_";
  sprintf(fileName, ".vcs/commits/commit_%d.txt",commitId);
  FILE * file=fopen(fileName, "r");
  if (!file) {
      printf("Failed to load commit %d.\n", commitId);
      return;
  }
  char line[256];
  int i=0;
  while (fgets(line, sizeof(line), file)) {
    if(i>2){
        char fileName[100];
        int hash;
        sscanf(line, "%s -> %d", fileName, &hash);
        printf("File: %s, Blob Hash: %d\n", fileName, hash);
        FILE * mainFile=fopen(fileName, "w");
        if(mainFile == NULL) {
            printf("Error opening file %s for writing.\n", fileName);
            continue;
        }
        char blobFileName[100];
        sprintf(blobFileName, ".vcs/blobs/%d.txt", hash);
        FILE * blobFile=fopen(blobFileName, "r");
        if(blobFile == NULL) {
            printf("Error opening blob file for hash %d.\n", hash);
            fclose(mainFile);
        }
        char buffer[bufferSize];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, sizeof(char), bufferSize, blobFile)) >0){
            fwrite(buffer, sizeof(char), bytesRead, mainFile);
        }
    }
    i++;

  }

}

// -------------------- Main --------------------
int main(int argc,char * argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command>\n",argv[0]);
        printf("Available commands: init, stage <file>, commit, load <commit_id>\n");
        return 1;
    } 



    if(strcmp(argv[1], "init") == 0){
         initRepo();
         printf("Initialized empty Git repository in .vcs\n"); }




    if(strcmp(argv[1], "stage") == 0){
        if(argc < 3) {
            printf("Usage: %s stage <file>\n", argv[0]);
            return 1;
        }
        FILE *indexFile = fopen(".vcs/index", "r");
        if (!indexFile) {
            printf("Index file doesn't exists.Please initialized a git repository before adding files to the staging area.\n");
            return 1;
            }
        stage stageArea[100]; 
        int stageCount = 0;
        char line[100];
        while (fgets(stageArea[stageCount].fileName, sizeof(stageArea->fileName), indexFile)) {
            strtok(stageArea[stageCount].fileName, "\n"); // 
            stageCount++;
        }
        fclose(indexFile);

        stageFile(stageArea, &stageCount, argv[2]);
        saveIndex(stageArea, stageCount);
        printf("Staged file: %s\n", argv[2]);
        return 0;
    }

    



    if(strcmp(argv[1], "commit") == 0){
        stage stageArea[100];
        int stageCount = 0;
        FILE *indexFile = fopen(".vcs/index", "r");
        if (!indexFile) {
            printf("Index file doesn't exists.Please initialized a git repository before committing files.\n");
            return 1;
        }
        char line[100];
        while (fgets(stageArea[stageCount].fileName, sizeof(stageArea->fileName), indexFile)) {
            strtok(stageArea[stageCount].fileName, "\n"); // Remove newline character
            stageCount++;
        }
        fclose(indexFile);

        if (stageCount == 0) {
            printf("No files staged for commit.\n");
            return 1;
        }

        int commitId = getNextCommitId();
        storeCommit(stageArea, stageCount, commitId);
        updateHead(commitId+1);
        printf("✅ Commit %d created successfully.\n", commitId);
        return 0;
    }

   if(strcmp(argv[1], "load") == 0){
        if(argc < 3) {
            printf("Usage: %s load <commit_id>\n", argv[0]);
            return 1;
        }
        int commitId = atoi(argv[2]);
        printf("Loading commit %d...\n", commitId);
        loadCommit(commitId);
        return 0;
    } 
    return 0;
}
