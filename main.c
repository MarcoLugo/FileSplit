#include <stdio.h>
#include <sys/stat.h>
#include <math.h>
#include <ctype.h>


void displayBanner(){
    printf("FileSplit v0.1 by Marco Lugo\n"); 
    printf("Description: splits binary files into n equally-sized files.\n"); 
    printf("Note:\n\tThis tool was created with the purpose of aiding in anti-virus analysis.\n\tThe author is not responsible for the consequences of use of this software.\n\n"); 
    return;
}

void displayHelp(char *ownFilename){
    printf("Usage:\n\n\t%s input_file output_directory n_new_files\n\n", ownFilename);
    printf("Example, splits myfile.exe into 25 files and saves them in subdirectory 'output':\n\n\t%s myfile.exe output 25\n\n", ownFilename); 
    return;
}

int doesFileExist(char *filename){
    struct stat buffer;   
    return(stat(filename, &buffer) == 0);
}


int main(int argc, char* argv[]){
    unsigned int fileCounter = 0;
    unsigned int chunkSize = 0;
    unsigned int remainderSize = 0;
    char fileCounterString[20];
    char filenameOUT[255] = "";
    struct stat info;
    FILE *fp;
    
    displayBanner();
    
    if(argc != 4) displayHelp(argv[0]);     
    else if(argc == 4){
        char *filenameIN = argv[1];
        char *dirOUT = argv[2];
        int splitSize = atoi(argv[3]);
        
        // Argument validation
        if(!doesFileExist(filenameIN)){
            printf("[!] ERROR: input file does not exist.\n");
            return -1;
        }
        if(splitSize <= 0){
            printf("[!] ERROR: size must be a positive integer.\n");
            return -1;
        }
        if(strlen(dirOUT) > 255 || strlen(filenameIN) > 255){ // This is to respect Windows's max name length (also should avoid buffer overflows when calling strcpy later on)
            printf("[!] ERROR: directory and filename length must be less than 255 characters.\n");
            return -1;
        }
        
        mkdir(dirOUT); // Create output directory
        
        // Retrieve file info, for size
        stat(filenameIN, &info);
        printf("[+] Input file size: %d bytes.\n", info.st_size);
        
        // Read contents of input file
        char *fileContent = (char *)malloc(info.st_size);
        fp = fopen(filenameIN, "rb");
        fread(fileContent, info.st_size, 1, fp);
        fclose(fp);

        // Determine the size of each new file
        chunkSize = ceil((float)info.st_size / (float)splitSize);
        remainderSize = info.st_size % splitSize;
        printf("[+] Creating %d new files of %d bytes each.\n", splitSize, chunkSize);
        printf("[+] Last file will contain %d bytes.\n", (chunkSize - remainderSize));
        
        // Write output file(s)
        for(fileCounter = 1; fileCounter <= splitSize; fileCounter++){
            strcpy(filenameOUT, dirOUT);
            strcat(filenameOUT, "/");
            strcat(filenameOUT, filenameIN);
            strcat(filenameOUT, ".");
            sprintf(fileCounterString, "%i", fileCounter);
            strcat(filenameOUT, fileCounterString);
            
            fp = fopen(filenameOUT, "wb");
            if (fp == NULL){
               printf("[!] ERROR: file %s could not be opened for writing, skipping...\n", filenameOUT);
               continue;
            }
            if(fileCounter == splitSize) chunkSize -= remainderSize; // Adjust size for the last file
    		fwrite(fileContent, 1, chunkSize, fp);
    		fclose(fp);
    		printf("[+] File #%d/%d (%d bytes): %s\n", fileCounter, splitSize, chunkSize, filenameOUT);
		}
    }

    return 0;
}
