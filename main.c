#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int liczba = 2;
int tab[2];

void sigusr1();
void sigint();
void sigquit();
void whoami();


int main()
{ int pid1, pid2;
    if (pipe(tab) == -1)
    {
        //zlapanie bledu przy tworzeniu potoku
        perror("pipe failure");
        exit(EXIT_FAILURE);
    }
    whoami(); //przedstawienie sie procesu


    if ((pid1 = fork())< 0)//wytworzenie pierwszego procesu potomnego
    {
        perror("Cannot fork!!\n"); //obsluga bledu jesli proces nie zostal stworzony
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0)
    {
        // obsluga procesu potomnego 1
        whoami(); //przedstawienie sie procesu
        signal(SIGINT,sigint); //ustawienie zachowania w przypadku ostrzymania SIGINT
        signal(SIGQUIT, sigquit); //ustawienie zachowania w przypadku ostrzymania SIGQUIT
        while(1){
            sleep(100); //nieskonczona petla, sleep aby nie obciazac procesora
        }

    }
    else
    {

        if ((pid2 = fork()) < 0) //wytworzenie procesu i obsluga bledu jesli proces nie zostal stworzony
        {
            perror("Cannot fork!!\n");
            exit(EXIT_FAILURE);
        }
        if (pid2 == 0){
            whoami(); //przedstawienie sie procesu
            signal(SIGUSR1,sigusr1);
            sleep(1);
            while(1){
                close(tab[0]);
                write( tab[1],&liczba,sizeof(liczba));
                printf("wyslalem %d, PID %d\n",liczba, getpid());
                liczba++;
                printf("Wyslalem powiadomienie SIGINT\n\n");
                kill(pid1,SIGINT);
                sleep(3); // 3 sekundy pauzy
                if(liczba == 5){

                    printf("\nMlodszy brat wysyla SIGQUIT\n\n");
                    kill(pid1,SIGQUIT);
                    sleep(1);
                    exit(EXIT_SUCCESS);
                }
            }
        }
        int status;
        waitpid(-1, &status, 0);
        while ((waitpid(-1, &status, 0)) > 0){ //oczekiwanie na procesy potomne
            sleep(3);
        }
    }



}


void sigusr1(){
    close(tab[0]);//zamkniecie odczytu
    write( tab[1],&liczba,sizeof(liczba));
    liczba++;
}
void sigint()

{  signal(SIGINT,sigint); // reset sygnalu
    printf("Dostalem SIGINT\n");
    int odczytana = 1;
    close(tab[1]); // zamkniecie zapisu
    read( tab[0], &odczytana,sizeof(liczba));
    printf("odczytalem %d, PID %d\n\n", odczytana, getpid());
    //.....

}

void sigquit()

{ printf("Zostalem unicestwiony przez brata\n");
    exit(EXIT_SUCCESS);
}

void whoami()
{
    printf("Moj PID %d, PID mojego ojca %d\n\n",getpid(),getppid());
    
}




