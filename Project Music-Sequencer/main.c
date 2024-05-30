#define _CRT_SECURE_NO_WARNINGS

#include "bst.h"
#include "note_io.h"
#include <string.h>
#include <sys/stat.h> //Needed to access the stat function.
                      //In this case, stat is used to check whether a file exists.

//The function checks if a file with the specified name exists.
//It returns 1 if the file exists and 0 if the file does not exist or an error occurred.
int file_exists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

//Generating a file name. If a file with this name already exists, an index is added to it. 
void generate_new_filename(const char* base_filename, char* new_filename) {
    int counter = 1;
    char temp_filename[256];
    sprintf(temp_filename, "%s.wav", base_filename);

    while (file_exists(temp_filename)) {
        sprintf(temp_filename, "%s_%d.wav", base_filename, counter);
        counter++;
    }

    strcpy(new_filename, temp_filename);
}

int main() {
    int choice, bar, semitones, i;
    double freq, index, time_shift;
    char note1[5], note2[5];
    BST_Node* root = NULL;
    BST_Node* new_note = NULL;
    BST_Node* t = NULL;
    char filename[1024];
    char out_filename[1024];
    char line[1024];
    FILE* f;

    // Read note names and frequencies from file "frequencies_of_notes.txt".
    read_note_table();

    choice = 0;
    while (choice != 2) {
        //MENU
        printf("----MENU----\n ");
        printf("1 > Create audio file\n ");
        printf("2 > Exit\n ");
        printf(">> ");

        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            //Inputs the inputfile name of the file containing the sheet music.
            //The file is specified without an filename extension.
            printf("Input name of the input file: \n> ");
            fgets(filename, 1024, stdin);
            strtok(filename, "\n"); 
            strcat(filename, ".txt");

            //Inputs the outputfile name.
            //The file is specified without an filename extension.
            printf("Input name of the output file: \n> ");
            fgets(out_filename, 1024, stdin);
            strtok(out_filename, "\n");

            generate_new_filename(out_filename, out_filename);

            f = fopen(filename, "r");
            if (f == NULL) {
                printf("Error: file doesn't open!\n");
            }
            else {
                //Reading a string from a file.
                while (fgets(line, 1024, f)) {
                    //Extracting bar and index.
                    sscanf(line, "%d %lf", &bar, &index);
                    i = strlen(line);

                    /*Newline character removal:
                    * If the last character of the string is a newline character ('\n'), 
                    * it is replaced with a null character ('\0').*/
                    if (line[i - 1] == '\n') line[i - 1] = '\0';

                    /*Note extraction: 
                    * If the penultimate character of a line is a tab ('\t'), the line is copied starting from the third character from the end. 
                    * Otherwise, the string is copied starting from the fourth character from the end to the note1 variable.*/
                    if (line[i - 4] == '\t') strcpy(note1, &line[i - 3]);
                    else strcpy(note1, &line[i - 4]);

                    //Frequency initialization: The note frequency (freq) is initialized to -1.
                    /*Note frequency search: 
                    * The note in the note_names array is searched.
                    * If a note is found, freq is set to the appropriate value from the note_freq array.*/
                    freq = -1;
                    for (i = 0; i < 100; i++) {
                        if (strcmp(note_names[i], note1) == 0) { freq = note_freq[i]; break; }
                    }

                    //Node creation and insertion: If freq is greater than 0, a new Binary Search Tree (BST) node with frequency, beat, and index is created and inserted into the tree.
                    if (freq > 0) {
                        new_note = newBST_Node(freq, bar, index);
                        root = BST_insert(root, new_note);
                    }
                }
                fclose(f);
            }
            //Creating a playlist, writing to a file, and clearing the binary tree.
            BST_makePlayList(root);
            play_notes(2, out_filename);
            delete_BST(root);
            root = NULL;
        }
    }
    //Clearing the tree before exiting the program.
    delete_BST(root);
    return 0;
}
