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

#211 page - The file Parsing Function
int parseFile( char * filename )
{
    FILE *fp;
    char *file, *cur;
    int fail = 0;

    extern int debug;

    file = (char *)malloc(MAX_FILE_SIZE);

    if ( file == NULL) return -1;

    fp = fopen(filename, "r");

    if ( fp == NULL )  
    {
        free ( file ) ;
        return -1;
    }

    fread ( file, MAX_FILE_SIZE, 1, fp );

    cur = &file[0];

    while ( 1 )
    {
        cur = strstr ( cur, "(defrule" );

        if ( cur == NULL )
        {
            fail = 1;
            break;
        }

        if ( !strncmp ( cur, "(defrule", 8 ))
        {
            int i = 0;

            cur += 9;

            while ( *cur != 0x0a )
            {
                ruleSet[ruleIndex].ruleName[i++] = *cur++;
            }
            ruleSet[ruleIndex].ruleName[i++]=0;

            cur = skipWhiteSpace( cur );

            # Parse the antecedents 
            cur = parseAntecedent ( cur, &ruleSet[ruleIndex])

            if (cur == NULL)
            {
                fail = 1;
                break;
            }

            #Should be sitting on the '==>'
            if ( !strncmp ( cur, "==>", 2))
            {
                cur = skipWhiteSpace ( cur + 2 );

                #parse the consequents 
                cur = parseConsequent ( cur, &ruleSet[ruleIndex]);

                if ( cur == NULL )
                {
                    fail = 1;
                    break;
                }

                if ( *cur == ')')
                {
                    cur = skipWhiteSpace ( cur + 1 )
                }
                else
                {
                    fail = 1;
                    break;
                }
            }
            else
            {
                fail = 1;
                break;
            }
            ruleSet[ruleIndex].active = 1;
            ruleIndex++;
        }

        else break;
    }
    if ( debug ) 
    {
        print ("Found %d rules\n", ruleIndex);
    }

    free ( ( void *)file );
    fclose( fp );
    return 0;
}

#213 page Antecedent and Consequent Parsing Function
char *parseAntecedent ( char *block, ruleType *rule )
{
    while ( 1 )
    {
        block = skipWhiteSpace ( block );

        if (*block == '(')  
            block = parseElement ( block, &rule->antecedent );
        else 
            break;
    }
    return block;
}

char *parserConsequent( char* block, ruleType *rule )
{
    while ( 1 )
    {
        block = skipWhiteSpace ( block );

        if ( *block == '(') 
            block = parseElement(block, &rule->consequent );
        else
            break;
    }
    return block;
}

#214 page - The parseElement Function
char *parseElement ( char *block, memoryElementType **met )
{
    memoryElementType *element;
    int i = 0;
    int balance = 1;

    element = (memoryElementType *)malloc ( sizeof (memoryElementType));
    element->element[i++] = *block++;

    while ( 1 )
    {
        if ( *block == 0 )
            break;
        if ( *block == ')' )
            balance--;
        if ( *block == '(')
            balance++;

        element->element[i++] = *block++;

        if (balance == 0) 
            break;
    }

    element->element[i] = 0;
    element->next = 0;

    if (*met == 0 )
        *met = element;
    else
    {
        memoryElementType *chain = *met;
        while ( chain->next != 0 )
            chain = chain->next;
        chain->next = element;
    }
    return block;
}

#215page - The whitespace consumer Function
char *skipWhiteSpace ( char *block )
{
    char ch;
    while ( 1 )
    {
        ch = *block;
        while (( ch != '(') && (ch != ')') && (ch != '=') && (ch != 0 ) && (ch != ';'))
        {
            block++;
            ch = *block;
        }

        if (ch == ';' )
            while( * block++ != 0x0a )
        else
            break;
    }

    return block;
}

#216 page - The interpret Function
void interpret ( void )
{
    int rule;
    int fired = 0;

    extern int checkRule ( int );
    extern int debug;

    for ( rule = 0; rule < MAX_RULES; rule++)
    {
        fired = 0;

        if ( ruleSet[rule].active )
        {
            fired = checkRule ( rule )
            /*
            if a rule had some effect on working memory, exit,
            otherwise test another rule.
            */
            if (fired)
                break;
        }
    }
    if (debug)
    {
        if (fired)  printf("Fired rule %s (%d)\n", ruleSet[rule].ruleName, rule);
    }

    return;
}

$217 page - The checkRule Function
int checkRule ( int rule )
{
    int fire = 0;
    char arg[MEMORY_ELEMENT_SIZE] = {0};

    extern int fireRule ( int, char * );

    fire  checkPattern ( rule, arg);

    if ( fire == 1 )
        fire = fireRule ( rule, arg );

    return fire;
}

#218 page - The Rule Matching Algorithm(checkPattern)
int checkPattern ( int rule, char * arg )
{
    int ret = 0;
    char term1[MEMORY_ELEMENT_SIZE + 1];
    char term2[MEMORY_ELEMENT_SIZE + 1];
    memoryElementType *antecedent = ruleSet[rule].antecedent;

    while ( antecedent )
    {
        sscanf ( antecedent->element, "(%s %s)", term1, term2);
        if (term2[strlen(term2)-1]== ')')
            term2[strlen(term2)-1] = 0;
        if (term2[0] == '?')
        {
            int i;
            char wm_term1[MEMORY_ELEMENT_SIZE + 1 ];
            char wm_term2[MEMORY_ELEMENT_SIZE + 1 ];

            for ( i = 0; i < MAX_MEMORY_ELEMENTS; i++ )
            {
                if ( workingMemory[i].active )
                {
                    sscanf ( workingMemory[i].element, "(%s %s)", wm_term1, wm_term2 );

                    if ( wm_term2[ strlen( wm_term2 ) - 1 ] ==')' )
                        wm_term2[ strlen( wm_term2 ) - 1] = 0;

                    if ( !stncmp ( term1, wm_term1, strlen( term1 ) ))
                        addToChain ( wm_term2 );

                }
            }
            antecedent = antecedent->next;
        }

        /*
            Now that we have the replacement strings, walk through
            the rules trying the replacement string when necessary.
        */

        do 
        {
            memoryElementType *curRulePtr, *temp;

            curRulePtr = ruleSet[rule].antecedent;

            while ( curRulePtr )
            {
                sscanf ( curRulePtr->element, "(%s %s)", term1, term2 );
                if ( term2[strlen(term2) - 1 ] == ')' )
                    term2[ strlen ( term2 ) - 1 ] = 0;
                if (!strncmp ( term1, "true", strlen ( term1 )))
                {
                    ret = 1;
                    break;
                }
                else
                {
                    if ( ( term2[0] == '?') && (chain) )
                        strcpy ( term2, chain->element );
                }
                ret = searchWorkingMemory ( term1, term2 );
                if ( !ret )
                    break;
                curRulePtr = curRulePtr->next;
            }

            if ( ret )
            {
                #Cleanup the replacement string chain
                while ( chain )
                {
                    temp = chain;
                    chain = chain->next;
                    free(tremp);
                }

                strcpy ( arg, term2 )

            }
            else
            {
                if ( chain )
                {
                    temp = chain;
                    chain = chain->next;
                    free ( temp );
                }
            }
        }
        while (chain);
    }
    return ret;
}

//221 page - Bulding a chain of Elements with addToChain
