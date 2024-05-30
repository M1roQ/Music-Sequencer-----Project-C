#pragma once

#ifndef NOTE_IO_H
#define NOTE_IO_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <stdint.h>

#define FS 44100 //The sampling frequency for note generation in Hz. 

//The data representing a single note
typedef struct note_struct{
	double freq; //Note's frequency
	int bar; //Which bar has a note
	double index; //Time index within the bar (from 0 to 1.0)
	double* waveform; //Dynamic array for waves
	double out_tminus1;		
	double previous_input;	
	int wave_length;
	int input_idx, output_idx;
	int n_sampled; //Counter of samples have been generated for this note. 
	               //Need to stop playing notes after a specified duration has been reached.
	struct note_struct* next; //A linked list for notes
} note;

// GLOBAL data
extern note* playlist_head;
char    note_names[100][5]; //Note names from the file "frequencies_of_notes.txt"
double  note_freq[100]; //Note frequencies from the file "frequencies_of_notes.txt"

void read_note_table(void);
note* new_note(double freq, int bar, double index);
note* playlist_insert(note* head, double freq, int bar, double index);
void delete_playlist(note* head);
double KS_string_sample(note* n);
void write_wav_header(FILE* f, unsigned int samples);
void play_notes(int bar_length, const char* filename);


#endif // NOTE_IO_H
