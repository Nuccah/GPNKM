#ifndef	__shmdef_h
#define	__shmdef_h

#include <sys/shm.h>

#define PERMS 0666

extern int errno;
extern char *sys_errlist[];


/*******************************************************************
 * shmcreate(size): creates a shared memory segment of length      *
 * "size" bytes and returns the shmid associated to this segment   *
 *******************************************************************/

int shmcreate(size)
     int size;
{
  int shmid;

  if ((shmid = shmget(IPC_PRIVATE,size,PERMS)) == -1)
    {
      fprintf(stderr,"error %d in shmget: %s\n",errno,sys_errlist[errno]);
      exit(errno);
    }
  return(shmid);
}

/*******************************************************************
 * shmattach(shmid): maps the shared memory segment associated     *
 * with the shared memory identifier specified by shmid into       *
 * the data segment of the calling process, at an address selected *
 * by the system, and returns this address.                        *
 *******************************************************************/

char *shmattach(shmid)
     int shmid;
{
  char *addr;

  addr = (char *)shmat(shmid,(char *)0,PERMS);
  if((int)addr  == -1)
    {
      fprintf(stderr,"error %d in shmat: %s\n",errno,sys_errlist[errno]);
      exit(errno);
    };
  return(addr);
}

/*******************************************************************
 * shmdetach(addr): unmaps from the calling process address space  *
 * the shared memory segment mapped at the address specified       *
 * by addr.                                                        *
 *******************************************************************/

void shmdetach(addr)
     char *addr;
{
  if(shmdt(addr) == -1)
    {
      fprintf(stderr,"error %d in shmdt: %s\n",errno,sys_errlist[errno]);
      exit(errno);
    }
}

/*******************************************************************
 * shmremove(shmid): removes the shared memory identifier          *
 * specified by shmid from the system.                             *
 *******************************************************************/

void shmremove(shmid)
     int shmid;
{
  if (shmctl(shmid,IPC_RMID,(struct shmid_ds *)0) == -1) 
    {
      fprintf(stderr,"error %d in shmctl: %s\n",errno,sys_errlist[errno]);
      exit(errno);
    }
}

#endif /* !__shmdef_h */
