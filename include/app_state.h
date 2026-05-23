#ifndef app_state_h
#define app_state_h

#define MAX_NOTES 10
#define NOTES_LEN 256

typedef struct {
    char notes[MAX_NOTES][NOTES_LEN];
    int notes_count;
} AppState;

#endif
