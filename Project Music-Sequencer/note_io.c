/*
Note synthesizer implementation. 
This file describes the functions of reading note frequencies, 
initializing a note from input values, inserting a note into a playlist, implementing the Karplus-Strong algorithm, writing a file header, and writing notes to a file.
*/

#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include "note_io.h"

#define PI 3.14159265358979323846

note* playlist_head = NULL;

//Read note names and frequencies from file "frequencies_of_notes.txt". Don't rename it!
void read_note_table(void){
    FILE *f;
    char line[1024];
    int idx;
   
    f = fopen("frequencies_of_notes.txt", "r");
    if (f == NULL){
      printf("Error: file doesn't open!\n");
      exit(EXIT_FAILURE);
    }
    
    idx = 0;
    while (fgets(&line[0], 1024, f))
    {
      note_names[idx][0] = line[0];
      note_names[idx][1] = line[1];

      if (line[2] != '\t') note_names[idx][2] = line[2];
      else note_names[idx][2] = '\0';

      note_names[idx][3] = '\0';

      if (line[2] == '\t') note_freq[idx] = strtod(&line[3], NULL);
      else note_freq[idx] = strtod(&line[4], NULL);
      
      idx++;
    }
}

//Creating and initializing a note from an input file that describe the note's frequency, and its position in the song.
note *new_note(double freq, int bar, double index){
    note *n;
    n = (note *)calloc(1, sizeof(note));

    if (!n){
        fprintf(stderr, "Out of memory!\n");
        exit(EXIT_FAILURE);
    }
 
    n->freq = freq;
    n->bar = bar;
    n->index = index;
    n->input_idx = 0;
    n->output_idx = 1;
    n->out_tminus1 = ((double)rand() / (double)RAND_MAX)-.5;
    n->n_sampled = 0;
    n->next = NULL;

    //Creating a dynamically allocated array to store the waveform of the note.
    //The size of this array should match the number of samples needed to represent a waveform at the note's frequency.
    //The formula is: length = sampling_rate / note_frequency.
    n->wave_length = round((double)FS / n->freq);
    n->waveform = (double *)calloc(n->wave_length, sizeof(double));

    if (!n){
    fprintf(stderr,"Ran out of memory allocating waveform.\n");
    return(NULL);
    }

    //Initializing the note's waveform. 
    for (int i = 0; i < n->wave_length; i++){
        *(n->waveform + i) = (1.0*(double)rand() / (double)RAND_MAX)-.5;
    }

    return(n);
}

//Inserting a new note at the requested location. 
note *playlist_insert(note *head, double freq, int bar, double index){
    note *n_n, *p;
    n_n = new_note(freq, bar, index);

    if (n_n == NULL) return head;
    if (head == NULL) return n_n;

    p = head;

    while (p->next != NULL) p = p->next;

    p->next = n_n;

    return head;  
}

//Releasing memory
void delete_playlist(note *head){
   note *t;
   
   while (head != NULL){
     t = head->next;
     free(head->waveform);
     free(head);
     head = t;
   }
}

/*Simulating the sound of a plucked string.
This function implements the Karplus-Strong algorithm to simulate the sound of a plucked string.
This is a way to simulate the vibrations of a string that includes a delay and feedback.
The delay simulates the string itself, and its length must match the wavelength of the string.
The feedback contains a low-pass filter that averages the two samples and passes them back to the input
with reduced gain. In the delay operation, the rightmost element is sent to the output each time and the others are shifted.
The input data for this process is taken from the feedback. */
double KS_string_sample(note *n){
    double output, new_input;

    n->n_sampled++;		// Keep track of how many times we've sampled this note

    output =* (n->waveform+n->output_idx);
    new_input = 0.999 * ((.25*n->out_tminus1) + (.75*output));

    //Low-pass filter
    double cutoff_frequency = 30000.0; //The cutoff frequency of the filter.
    double RC = 1.0 / (cutoff_frequency * 2 * PI); //Filter time constant, which is calculated by the formula for RC-chain in a low-pass filter. 
                                                   //The smaller the RC, the faster the filter responds to changes in the input signal.
    double alpha = 1.0 / (1.0 + RC * FS); //Filter smoothing factor, which determines how much the new input signal affects the filter output. 
                                          //An alpha value close to 1 means that the influence of the new input is greater, while a value close to 0 means that the influence of the previous output is greater.
    
    new_input = alpha * new_input + (1 - alpha) * n->previous_input;
    n->previous_input = new_input;

    // Update delay length based on note frequency
    double frequency = n->freq;  // Adjust as needed
    int delay_length = (int)(FS / frequency);
    if (delay_length < 1) delay_length = 1;  // Ensure delay length is at least 1

    // Update internal state
    n->out_tminus1 = output;
    *(n->waveform+n->input_idx) = new_input;

    n->output_idx++;
    if (n->output_idx == n->wave_length) n->output_idx = 0;
    n->input_idx++;
    if (n->input_idx == n->wave_length) n->input_idx = 0;

    return(output);
}

//Writing out the .wav header for the output file.
void write_wav_header(FILE *f, unsigned int samples){
    char txt[250];
    uint32_t chunk2size;
    uint32_t tmp;
    unsigned char tmp2;
    // Compute the chunk size in bytes
    chunk2size = samples * 2 * 16 / 8; //samples, 2 channels, 16 bits/channel, 8 bits per byte

    strcpy(&txt[0], "RIFF"); //RIFF (Resource Interchange File Format) header
    fwrite(&txt[0], 4*sizeof(unsigned char), 1, f);
    //Writing the size of the entire file minus the 8 bytes for "RIFF" and file size
    tmp = chunk2size + 36; //36 bytes for headers excluding the data chunk
    fwrite(&tmp, 4 * sizeof(unsigned char), 1, f);

    strcpy(&txt[0], "WAVE"); //Wave file identifier
    fwrite(&txt[0], 4 * sizeof(unsigned char), 1, f);

    strcpy(&txt[0], "fmt "); //Format section
    fwrite(&txt[0], 4 * sizeof(unsigned char), 1, f);
    tmp = 16;
    fwrite(&tmp, 4 * sizeof(unsigned char), 1, f);
    tmp2 = 1;
    fwrite(&tmp2, sizeof(unsigned char), 1, f);
    tmp2 = 0;		
    fwrite(&tmp2, sizeof(unsigned char), 1, f);
    tmp2 = 2;
    fwrite(&tmp2, sizeof(unsigned char), 1, f);
    tmp2 = 0;		
    fwrite(&tmp2, sizeof(unsigned char), 1, f);
    tmp = FS; //Writing the sample rate
    fwrite(&tmp, 4 * sizeof(unsigned char), 1, f); //Sampling rate
    tmp = FS * 2 * 16 / 8;	//Byte rate (sampling rate * num channels * bits/channel / 8 bits)				
    fwrite(&tmp, 4 * sizeof(unsigned char), 1, f); 
    tmp2 = 2 * 16 / 8; //Block alignment (number of channels * bits per sample / 8)
    fwrite(&tmp2, sizeof(unsigned char), 1, f);
    tmp2 = 0;
    fwrite(&tmp2, sizeof(unsigned char), 1, f);
    tmp2 = 16; //Bits per sample
    fwrite(&tmp2, sizeof(unsigned char), 1, f);
    tmp2 = 0;
    fwrite(&tmp2, sizeof(unsigned char), 1, f); 

    strcpy(&txt[0], "data"); //Data field follows
    fwrite(&txt[0], 4 * sizeof(unsigned char), 1, f); 
    //Writing the size of the data chunk
    fwrite(&chunk2size, 4 * sizeof(unsigned char), 1, f);
}

/*The main synthesis function.
It starts a time counter from bar=1, idex=0 and plays out the notes in the song at the specified times. 
Notes are placed in a temporary linked list and pointers are used to keep track of which notes are active at any given moment.
bar_length indicates the duration in seconds for each bar, and controls the overall speed of playback. */
void play_notes(int bar_length, const char* filename){
    note *q, *st, *ed;
    int bar, max_bar, max_length, s_idx;
    double index;
    unsigned int sample_idx, max_sample_idx;
    int16_t this_sample_int;
    double this_sample, this_sample_r, this_sample_l;
    double balance,fAmp;
    int done;
    FILE *f;

    if (playlist_head == NULL){
        printf("Input playlist is empty!\n");
        return;
    }

    max_bar = 0;
    q = playlist_head;

    while (q != NULL){
        max_bar = q->bar;
        q = q->next;
    }

    max_length = 3 * FS; // Maximum note length, number of seconds * sampling rate.

    //Calculating song length in samples 
    max_sample_idx = bar_length * FS * (max_bar + 1);

    fprintf(stderr, "\nThe song will be written to the '%s' file.\nPlease wait.\n\n", filename); 

    f = fopen(filename, "wb+");	// Open output file (.wav) for writing.
    if (f){
        //Main loop of the synthesis part. 
        //Monitors the current bar:index position and plays any notes that are active within this interval.
        write_wav_header(f,max_sample_idx);

        st = ed = playlist_head;
        sample_idx = 0; 

        for (bar = playlist_head->bar; bar <= max_bar; bar++){
            for (index=0; index <= 1.0; index += 1.0 / (bar_length * 44100.0)){
                
                done = 0;
                while(!done){
                    done = 1;
                    if (ed->next != NULL){
                        if ((double)ed->next->bar + ed->next->index <= (double)bar + index){
	                        ed = ed->next;
	                        done = 1;
                        }
                    }
                }
    
                //For each note, generate any needed samples, and output to file

                //Need to generate all the samples for this index
                if (st!=NULL){
                    q=st;
                    this_sample_l=0;
                    this_sample_r=0;

                    while(ed->next != q){
                        balance = (q->freq - 525.0);

                        if (balance < 0) balance = (525.0 + balance) / 525.0;

                        else balance = balance / (5000.0 - 525.0);

                        fAmp = (pow(q->freq, .33) / 18.0); //Amplitude boost for high frequencies notes
                        this_sample = KS_string_sample(q) * fAmp;
                        this_sample_l += (balance) * this_sample;
                        this_sample_r += (1.0 - balance) * this_sample;
                        q = q->next;
                    }

                    //Transforming to a 2-byte signed integer for each channel, and write to file
                    this_sample_r = tanh(this_sample_r);
                    this_sample_int = (int16_t)(this_sample_r * 32700);
                    fwrite(&this_sample_int, 2 * sizeof(unsigned char), 1, f); //R channel
                    this_sample_l = tanh(this_sample_l);
                    this_sample_int = (int16_t)(this_sample_l * 32700);
                    fwrite(&this_sample_int, 2 * sizeof(unsigned char), 1, f); //L channel

                    sample_idx++;
                }

                //Fill out the remainder of the file with zeros.
                else{
                    for (s_idx = 0; s_idx < 44100; s_idx++){
                        this_sample_int = 0;
                        fwrite(&this_sample, 2 * sizeof(unsigned char), 1, f); //L channel
                        fwrite(&this_sample, 2 * sizeof(unsigned char), 1, f); //R channel
                        sample_idx++;
                    }
                }
 
                //Checking if notes have ended 
                done = 0;
                while (!done){
                    done = 1;
                    if (st != NULL && st != ed->next)
                    if (st->n_sampled > max_length){
                    st = st->next;
                    done = 0;
                    }       
                }

                if (sample_idx > max_sample_idx) break;
            }
            if (sample_idx > max_sample_idx) break;
        }
	
        fclose(f);
    }
    else fprintf(stderr, "Unable to open file for output!\n");

    //Cleaning playlist
    delete_playlist(playlist_head);
    playlist_head = NULL;
}
