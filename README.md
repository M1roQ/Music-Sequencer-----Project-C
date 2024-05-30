# Music Sequencer README

## 1. Project Description
This project is a music sequencer written in C that converts a text file describing a musical composition into an audio file in the `.wav` format. The sequencer processes note data, constructs a binary search tree (BST) to order the notes, and synthesizes audio using the Karplus-Strong algorithm. It consists of the following files: `main.c`, `note_io.c`, `note_io.h`, `bst.c`, `bst.h`.

## 2. Features
- Converts text-based musical notation into audio.
- Outputs a `.wav` file with the generated audio.
- Uses a BST to organize and process musical notes efficiently.
- Implements the Karplus-Strong algorithm for string synthesis.

## 3. Program Files and Functions

### 3.1 main.c
The entry point of the program. It handles user input and controls the main flow of the program.

#### Functions:
- **int file_exists(const char* filename):** Checks if a file exists.
  - **Parameters:** `const char* filename` - Path to the file.
  - **Returns:** `1` if the file exists, `0` otherwise.
  
- **void generate_new_filename(const char* base_filename, char* new_filename):** Generates a unique filename by appending an index if the file already exists.
  - **Parameters:** `const char* base_filename` - Base filename, `char* new_filename` - Buffer to store the new filename.
  - **Returns:** None.
  
- **int main():** Main function that provides the user interface and orchestrates the program flow.
  - **Parameters:** None.
  - **Returns:** `0` on successful execution.

### 3.2 note_io.h
Header file for note input/output and synthesis functions.

#### Functions:
- **void read_note_table(void):** Reads note names and frequencies from the file "frequencies_of_notes.txt".
  - **Parameters:** None.
  - **Returns:** None.
  
- **note* new_note(double freq, int bar, double index):** Creates and initializes a new note.
  - **Parameters:** `double freq` - Note frequency, `int bar` - Bar number, `double index` - Time index within the bar.
  - **Returns:** Pointer to the new note.
  
- **note* playlist_insert(note* head, double freq, int bar, double index):** Inserts a new note into the playlist.
  - **Parameters:** `note* head` - Head of the playlist, `double freq` - Note frequency, `int bar` - Bar number, `double index` - Time index within the bar.
  - **Returns:** Updated head of the playlist.
  
- **void delete_playlist(note* head):** Deletes the entire playlist and frees memory.
  - **Parameters:** `note* head` - Head of the playlist.
  - **Returns:** None.
  
- **double KS_string_sample(note* n):** Generates a sample for a note using the Karplus-Strong algorithm.
  - **Parameters:** `note* n` - Pointer to the note.
  - **Returns:** The generated sample as a `double`.
  
- **void write_wav_header(FILE* f, unsigned int samples):** Writes the WAV file header.
  - **Parameters:** `FILE* f` - File pointer, `unsigned int samples` - Number of samples.
  - **Returns:** None.
  
- **void play_notes(int bar_length, const char* filename):** Synthesizes and writes notes to a WAV file.
  - **Parameters:** `int bar_length` - Length of a bar in seconds, `const char* filename` - Output file name.
  - **Returns:** None.

### 3.3 note_io.c
Implementation of note input/output and synthesis functions.

### 3.4 bst.h
Header file for binary search tree (BST) operations.

#### Functions:
- **BST_Node* newBST_Node(double freq, int bar, double index):** Creates and initializes a new BST node.
  - **Parameters:** `double freq` - Note frequency, `int bar` - Bar number, `double index` - Time index within the bar.
  - **Returns:** Pointer to the new BST node.
  
- **BST_Node* BST_insert(BST_Node* root, BST_Node* new_node):** Inserts a new node into the BST.
  - **Parameters:** `BST_Node* root` - Root of the BST, `BST_Node* new_node` - Node to be inserted.
  - **Returns:** Updated root of the BST.
  
- **void BST_makePlayList(BST_Node* root):** Traverses the BST and generates an ordered playlist.
  - **Parameters:** `BST_Node* root` - Root of the BST.
  - **Returns:** None.
  
- **void delete_BST(BST_Node* root):** Deletes the BST and frees memory.
  - **Parameters:** `BST_Node* root` - Root of the BST.
  - **Returns:** None.

### 3.5 bst.c
Implementation of binary search tree (BST) operations.

## 4. Program Workflow

1. **Initialization:**
   - The program starts by reading note frequencies from "frequencies_of_notes.txt".
   
2. **User Interaction:**
   - The user is presented with a menu to either create an audio file or exit.
   - If the user chooses to create an audio file, they are prompted to input the name of the file containing the sheet music (without extension) and the name of the output file (without extension). If a file with this name already exists, the program generates a new name by adding an index to the base name using the generate_new_filename function.

3. **File Processing:**
   - The input file is read line by line. Each line contains a note's bar number, time index, and note name.
   - The note frequency is looked up, and a new BST node is created for each note.
   - The nodes are inserted into the BST based on their bar and index.

4. **Playlist Generation:**
   - The BST is traversed in order, and notes are inserted into a playlist.

5. **Audio Synthesis:**
   - The notes in the playlist are processed using the Karplus-Strong algorithm to generate audio samples.
   - The samples are written to a WAV file.

6. **Cleanup:**
   - Memory allocated for the BST and playlist is freed before the program exits.

## 5. Examples

### Input File (example_input.txt)
The input file contains the sheet music with each line specifying a note, its bar, and its time index within the bar. All values in the string must be separated by a tab character. An example format:
```
0 0.0 C4
0 0.5 E4
0 1.0 G4
1 0.0 C5
```
The project contains three examples of input files: test1.txt , test2.txt and test3.txt.

### Output File
The output file is a WAV audio file named as specified by the user. The project contains an example output file called test3.wav.

## 6. Dependencies
 - <stdio.h>: Standard input and output functions.
 - <stdlib.h>: General utilities, including memory management.
 - <string.h>: String handling functions.
 - <math.h>: Mathematical functions.
 - <stdint.h>: Fixed-width integer types.
 - <sys/stat.h>: Provides the stat function to check file status, used to determine if a file exists.

## 7. Compilation
1. To go to the project directory, enter the command:
```sh
cd Project/Music-Sequencer\
```
2. To compile the program, use the following command:
```sh
gcc -o sequencer main.c note_io.c bst.c -lm
```

## 8. Running the Program
To run the program, use the following command:
```sh
./sequencer
```
## 9. Example Usage
1. Run the program.
2. To create a file, select programs, select "Create audio file" by typing "1".
3. Input the name of the text file with sheet music (e.g., song).
4. Input the desired name for the output WAV file (e.g., output).
5. The program processes the input, generates the audio, and saves it to the specified output file. If output.wav already exists, a new name like output_1.wav will be generated.
6. To exit the program, select "Exit" by entering "2".
