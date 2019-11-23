#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(void)
{
    time_t     now=1574101356, now1=1574051034;
    struct tm  ts;
    char       buf[80];


    // Get current time

    ts=*gmtime(&now);
   
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &ts);
   // year_pr = ts.tm_year;
    printf("Local Time %s\n", buf);


    struct tm  ts1;
    char       buf1[80];


    
    ts1 = *gmtime(&now1);
    strftime(buf1, sizeof(buf1), "%a %Y-%m-%d %H:%M:%S", &ts1);
   // year_pr = ts.tm_year;
    printf("Local Time %s\n", buf1);

    return 0;
}