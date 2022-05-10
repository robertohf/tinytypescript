#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <iostream>

using namespace std;

extern FILE *yyin;
extern FILE *fp;

DIR *dir;
struct dirent *ent;

//int yyparse();
int yylex();
void yyrestart(FILE *input_file);
bool execute(const char* argument);

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Missing input file(s). %s \n", argv[0]);
        return 1;
    }

    bool parse_successfully = execute(argv[1]);
    if(!parse_successfully)
        cout << "--COMPILED SUCCESSFULLY" << endl;
    else
        cout << "--COMPILED FAILED" << endl;
    return 0;
}

bool execute(const char* directory) {
    bool yyin_flag = true;
    int parse_successfully = 0;
    std::string dir_path = directory;
    std::string file_path;

    if((dir = opendir(dir_path.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            if((strstr(ent->d_name, ".ts") != NULL)) {
                file_path.clear();
                file_path = dir_path.append(ent->d_name);
                
                fp = fopen(file_path.c_str(), "r");
                if(fp == NULL) {
                    printf("File Error. Null.");
                    return 1;
                }

                if (yyin_flag) {
                    yyin = fp;
                    yyin_flag = false;
                }

                /* parse_successfully = yyparse(); */
                int token;
                while ((token = yylex()) != 0) {
                    cout << token << endl;
                }

                yyrestart(yyin);
                fclose(fp);
            }
        }
        closedir(dir);
    }
    return parse_successfully;
}