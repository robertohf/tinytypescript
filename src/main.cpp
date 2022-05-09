#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <iostream>

using namespace std;

extern FILE *yyin;
extern FILE *fp;

DIR *dir;
struct dirent *ent;

int yyparse();
void yyrestart(FILE *input_file);

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Missing input file(s). %s \n", argv[0]);
        return 1;
    }

    bool yyin_flag = true;
    const char *dir_path = argv[1];
    char file_path[100];
    sprintf(file_path, "%s", dir_path);

    if((dir = opendir(dir_path)) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            if((strstr(ent->d_name, ".sql") != NULL)) {
                memset(file_path, 0, sizeof(char*));
                sprintf(file_path, "%s", dir_path);
                sprintf(file_path, "%s", ent->d_name);
                
                fp = fopen(file_path, "r");
                if(fp == NULL) {
                    printf("File Error. Null.");
                    return 1;
                }

                if (yyin_flag) {
                    yyin = fp;
                    yyin_flag = false;
                }

                yyparse();

                yyrestart(yyin);
                fclose(fp);
            }
        }
        closedir(dir);
    } 
    cout << "\x1b[--COMPILED SUCCESSFULLY \x1b[0m" << endl;
    return 0;
}