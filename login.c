#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

volatile sig_atomic_t sig_recived = 0;

void sig_handler(int sig) {
    (void)sig;
    sig_recived = 1;
    write(STDOUT_FILENO, "\n", 1);
}

int validate_email(const char *email) {
    if (email == NULL) return 0;

    int len = strlen(email);
    if (len < 5 || len > 254) return 0; // দৈর্ঘ্য চেক

    // স্টেট ট্র্যাকিং ভেরিয়েবল
    enum { START, LOCAL, DOT_LOCAL, AT, DOMAIN, DOT_DOMAIN, TLD } state = START;
    int tld_len = 0;

    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)email[i];

        switch (state) {
            case START:
                if (islower(c) || isdigit(c)) state = LOCAL;
                else return 0; // শুরু অবশ্যই বর্ণ বা সংখ্যা
                break;

            case LOCAL:
                if (c == '@') state = AT;
                else if (c == '.') state = DOT_LOCAL;
                else if (islower(c) || isdigit(c) || c == '_' || c == '-') state = LOCAL;
                else return 0;
                break;

            case DOT_LOCAL:
                if (islower(c) || isdigit(c)) state = LOCAL;
                else return 0; // ডটের পর বর্ণ/সংখ্যা বাধ্যতামূলক (পরপর ডট বা .@ নিষিদ্ধ)
                break;

            case AT:
                if (islower(c) || isdigit(c)) state = DOMAIN;
                else return 0; // @ এর পরেই ডট বা হাইফেন নিষিদ্ধ
                break;

            case DOMAIN:
                if (c == '.') state = DOT_DOMAIN;
                else if (islower(c) || isdigit(c) || c == '-') state = DOMAIN;
                else return 0;
                break;

            case DOT_DOMAIN:
                if (islower(c)) { state = TLD; tld_len = 1; }
                else if (isdigit(c)) state = DOMAIN;
                else return 0;
                break;

            case TLD:
                if (islower(c)) { tld_len++; }
                else if (c == '.') { state = DOT_DOMAIN; tld_len = 0; }
                else return 0;
                break;
        }
    }

    // স্টেট TLD হতে হবে এবং শেষের অংশ (যেমন .com) অন্তত ২ অক্ষরের হতে হবে
    return (state == TLD && tld_len >= 2);
}

int main() {
    char pass[100];
    int i = 0, last_char = 0;
    char ch;
    char email[100];
    size_t len = 0;
   struct termios oldt, newt; //create oldt and newt ver on termios


   //signal rules..

   struct sigaction sa;
   sa.sa_handler = sig_handler;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = 0;
   sigaction(SIGINT, &sa, NULL);
   

   while (1)
   {
        printf("Enter Your Email: ");
        fgets(email, sizeof(email), stdin);
        email[strcspn(email, "\n")] = 0;
        int is_email = validate_email(email);
        if (is_email == 0)
        {
            printf("\033[1;31mInvalid email!\033[0m\n");
        } else {
            break;
        }
        
    }
    

   //terminal on cononical mode
   tcgetattr(STDIN_FILENO, &oldt); //get defalut terminal settings on oldt
   newt = oldt;                     // copy all on newt because if i dont copy oldt's value can broke.
   newt.c_lflag&= ~(ICANON|ECHO); // local flag using trun off icanon for input persing without pressing enter and off echo for off printing on screen.
   tcsetattr(STDIN_FILENO, TCSANOW, &newt); //now set newt for raw mode tcsanow say that is's do immidiatly.
    // now terminal on raw mode
    printf("Enter Password: ");
    fflush(stdout);
    ssize_t n;

    while (1)
    {
        
        n = read(STDIN_FILENO, &ch, 1);

        if (n < 0)
        {
            if (errno == EINTR)
            {
                if (sig_recived)
                {
                    sig_recived = 0;
                    pass[0] = '\0';
                    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                    return 1;
                }
                continue;
                
            }
            break;
            
        }

        if (ch == 4) {
            if (i == 0) {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                return 0;       // exit shell
            }
            break;              // accept typed line
        }
        


        if (ch == 127 || ch == '\b')
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
                fflush(stdout);
            }
            last_char = ch;
            continue;
            
        }

        

        if (ch == '\n')
        {
            write(STDOUT_FILENO, "\n", 1);
            break;
        }
        
        

        if (i < sizeof(pass) - 1)
        {
            pass[i++] = ch;
        }
       last_char = ch; 
    }
    pass[i] = '\0';

    
    


    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    char get_pass[100], get_email[100];

    
    FILE *ptr = fopen("data.txt", "r");
    if (ptr == NULL)
    {
        printf("Fialed to open file!\n");
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return 1;
    }
    
    int flag = 0;
    while (fscanf(ptr, "%s %s", get_email, get_pass) == 2)
    {
        if (strcmp(email, get_email) == 0 && strcmp(pass, get_pass) == 0)
        {
           flag = 1;
           break;
        } 
        
    }
    
    fclose(ptr);

    if (flag)
    {
        printf("\033[1;32mLogin Successfull!\033[0m\n");
    } else {
        printf("\033[1;31mTry again!\033[0m\n");
    }
    
    
    
    return 0;
}