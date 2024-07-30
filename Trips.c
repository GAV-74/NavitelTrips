#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include <wchar.h>

#pragma pack(1)
struct		//track data
{uint8_t r1;	//unused - 0xFF
 uint32_t lon;	//longtitude
 uint32_t lat;	//latitude
 uint32_t uknwn;// - ?
 int32_t ele;	//elevation*10
 time_t t;	//time
 uint32_t v;	//velocity*10
 uint8_t hdop;	//HDOP*10
 uint8_t vdop;	//VDOP*10
 uint8_t nsat;	//used satellites count
 uint8_t fix;	//fix type - ?
} t;
struct		//start/finish data
{time_t t;	//time
 uint32_t lat;	//latitude
 uint32_t lon;	//longtitude
 uint8_t len;	//wstring length
} d;
uint8_t l;
uint32_t i,x,dist,sp;
char ofn[256],st[256],br;
wchar_t adr[256];

void wprint(wchar_t *s,uint8_t l)
{while(*s&&l--) putchar(wctob(*s++));
}

int main(int argc,char *argv[])
{FILE *fi,*fo;
 setlocale(LC_ALL,"");
 printf("Navitel Trip to OziExplorer Track file converter v.0.3a by Alexander Gatalsky\n");
 if(argc<2) fi=fopen("Trips.bin","rb"); else fi=NULL;
 if(fi!=NULL)
   {fseek(fi,4,SEEK_CUR);	//skip type1 record
    fread(&x,sizeof(x),1,fi);	//records counts
    printf("\nTrips.bin (%u record%s):\n\n",x,(x==1)? "":"s");
    for(i=0;i<x;i++)
      {fseek(fi,4,SEEK_CUR);	//skip type2 record
       fread(&d,sizeof(d),1,fi);//read start record
       strftime(st,sizeof(st),"%d-%m-%Y,%T",localtime(&d.t));
       printf("Start: %s %.8f %.8f\n",st,(float)d.lat/5965232.0,(float)d.lon/5965232.0);
       fread(adr,d.len*2,1,fi);	//read start address
       wprint(adr,d.len);
       fread(&d,sizeof(d),1,fi);//read finish record
       strftime(st,sizeof(st),"%d-%m-%Y,%T",localtime(&d.t));
       printf("\nFinish: %s %.8f %.8f\n",st,(float)d.lat/5965232.0,(float)d.lon/5965232.0);
       fread(adr,d.len*2,1,fi);	//read finish address
       wprint(adr,d.len);
       printf("\nDistance: ",dist);
       fread(&dist,sizeof(dist),1,fi);//read distance
       if(dist<1000) printf("%um",dist);
       else printf("%.3fkm",(float)dist/1000.0);
       fread(&sp,sizeof(sp),1,fi);//read average speed
       printf(", Average speed: %ukm/h",sp);
       printf("\nFile: ");
       fseek(fi,4*4+1,SEEK_CUR);//skip bounds and zero byte
       fread(&l,sizeof(l),1,fi);//read wstring length
       fread(adr,l*2,1,fi);	//read filename
       wprint(adr,l);
       printf("\n\n");
      }
    fclose(fi);
   }
//
 if(argc<2) fi=fopen("TripData.bin","rb"); else fi=NULL;
 if(fi!=NULL)
   {fseek(fi,4,SEEK_CUR);	//skip type1 record
    fread(&x,sizeof(x),1,fi);	//records count
    printf("\nTripData.bin (%u record%s):\n",x,(x==1)? "":"s");
    for(i=0;i<x;i++)
      {fread(&d,sizeof(d),1,fi);//read start record
       strftime(st,sizeof(st),"%d-%m-%Y,%T",localtime(&d.t));
       printf("\n%s %.8f %.8f\n",st,(float)d.lat/5965232.0,(float)d.lon/5965232.0);
       fread(adr,d.len*2,1,fi);	//read start address
       wprint(adr,d.len);
       printf("\nFile: ");
       fseek(fi,24+1,SEEK_CUR);	//skip bounds and zero byte
       fread(&l,sizeof(l),1,fi);//read wstring length
       fread(adr,l*2,1,fi);	//read filename
       wprint(adr,l);
       printf("\n");
      }
    fclose(fi);
    printf("\n");
   }
//
  printf("\n");
 if(argc<2)
   {printf("Usage: %s <infile>\n",argv[0]);
    return 1;
   }
 fi=fopen(argv[1],"rb");
 if(fi==NULL)
   {printf("Can't open input file %s\n",argv[1]);
    return 2;
   }
 strcpy(ofn,argv[1]);
 strcat(ofn,".plt");
 fo=fopen(ofn,"wt");
 if(fo==NULL)
   {printf("Can't open output file %s\n",ofn);
    fclose(fi);
    return 3;
   }
 fprintf(fo,"OziExplorer Track Point File Version 2.1\n"
            "WGS 84\n"
            "Altitude is in Feet\n"
            "Reserved\n"
            "0,2,255,R,1,0,0,255\n"
            "0\n");
 printf("%s -> %s... ",argv[1],ofn);
 fseek(fi,4,SEEK_CUR);		//skip first 4 bytes
 br='1';			//break at first track record
 while(fread(&t,sizeof(t),1,fi)==1)
   {strftime(st,sizeof(st),"%d-%m-%Y,%T",localtime(&t.t));
    fprintf(fo,
            "%.8f,%.8f,%c,%.3f,%.8f,%s,%.1f,%.1f,%.1f,%.1f\n",
	    (double)t.lat/186413.5,(double)t.lon/186413.5,br,(double)t.ele/3.05,
	    (double)t.t/86400.0+25569.0,st,(double)t.v/10.0,
	    sqrt((double)t.hdop*(double)t.hdop+(double)t.vdop*(double)t.vdop)/10.0,
	    (double)t.hdop/10.0,(double)t.vdop/10.0);
    br='0';			//normal track record
   }
 fclose(fi);
 fclose(fo);
 printf("Done\n");
 return 0;
}
