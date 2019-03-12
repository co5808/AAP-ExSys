#207 page
#define MEMORY_ELEMENT_SIZE 80
#define MAX_MEMORY_ELEMENTS 40
#define MAX_RULES           40
#define MAX_TIMERS          10

typedef struct memoryElementStruct *memPtr;

#memoryElementStruct Linked List 구조체 선언
typedef struct memoryElementStruct 
{
    int active;
    char element[MEMORY_ELEMENT_SIZE + 1]
    struct memoryElementStruct *next
} memoryElementType;

typedef struct
{
    int active;
    char ruleName[MEMORY_ELEMENT_SIZE + 1]
    memoryElementType *antecedent;
    memoryElementType *conseuent;
} ruleType;

typedef struct
{
    int active;
    int expiration;
} timerType;

#208 Page _ Global structures Declaration
memoryElementType workingMemory[MAX_MEMORY_ELEMENTS];

ruleType ruleSet[MAX_RULES];

timerType timers[MAX_RULES];

int endRun = 0;
int debug = 0;

#208 page - The rules-based System "main" function.
int main ( int argc, char *argv[] )
{
    int opt, ret;
    char inpfile[80] = {0};

    extern void processTimers( void );
    extern int parseFile ( char *);
    extern void interpret( void )

    while ( ( opt = getopt(argc, argv, "hdr:")) != -1 )
    {
        switch ( opt )
        {
            case 'h':
                emitHelp()
                break;
            case 'b':
                debug = 1;
                print("Debugging enabled\n");
                break;
            case 'r':
                strcpy(inpfile, optarg);
                break;         
        }
    }

    if (inpfile[0] == 0 ) emitHelp();

    bzero( ( void * )workingMemory, sizeof ( workingMemory) );
    bzero( ( void * )ruleSet, sizeof(ruleSet) );
    bzero( (void * )timers, sizeof(timers) );

    ret = parseFile( inpfile );

    if (ret < 0 )
    {
        printf("\nCould not open file, or parse error\n\n");
        exit(0);
    }

    while ( 1 )
    {
        interpret();

        if (debug) printWorkingMemory();

        processTimers();

        if (endRun) break;

        sleep(1);
        
    }
    return 0;
}