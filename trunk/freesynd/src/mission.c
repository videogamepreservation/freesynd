#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef unsigned char  Uint8;
typedef   signed char  Sint8;
typedef unsigned short Uint16;
typedef   signed short Sint16;

#define swaw(x) (((x) << 8) | ((x) >> 8))


struct unk01 {
  Uint8 unkn10[8];
} __attribute__((packed));

struct unk02 {
  Uint8 unkn10[256];
} __attribute__((packed));

// This structure contains all people in the game, including agents
// This struct size is 92.
struct ped {
  Uint8 unkn2[4];
  Uint16 px;
  Uint16 py;
  Uint16 pz;
  Uint8 unkn3;                    // this bit may be connected with minimap in bottom left
  Uint8 unkn4;                    // this bit is connected with control
  Uint8 unkn5[2];                 // nothing changes when this changes
  Uint16 vistype;               // visual object type;must correspond to right type
  Uint16 currvistype;   // actual character picture; game changes this automaticaly after any command
  Uint8 unkn6;                    // shoot distance? if this is large, computer just follows me and won't shoot until I come closer
  Uint8 unkn7[5];                 // ??
  Uint8 objtype;                  // object type on minimap and for computer?; 1-human, 2-car
  Uint8 unkn8[5];                 // ??
  Uint8 unkn9[8];                 // somewhere here is written to who the agent belongs
  Uint8 unkn10[4];
  Uint8 unkn11[4];
  // Half of structure - 46 bytes
  Uint8 unkn12[12];
  Uint8 unkn13[13];
  // IPA levels: white bar level,set level,exhaused level and forced level
  Uint8 adrenalevels[4];
  Uint8 intelilevels[4];
  Uint8 perceplevels[4];
  Uint8 unkn14;                   // This value is different for most objects
  Uint8 unkn101[8];               // ??
} __attribute__((packed));

// This structure contains all cars in the game, including destroyed
// This struct size is 42.
struct car {
  Uint8 unkn2[4];
  Uint8 mapposx[2];
  Uint8 mapposy[2];
  Uint8 mapposz[2];
  Uint8 unkn3;                    // if not set to 4, there is no car
  Uint8 unkn4[5];                 // somewhere here is damage made to car (filled with 122, one shot destroys car)
  Uint8 currvistype;              // actual car picture; changed automatically when car is moving
  Uint8 unkn6;                    // car is not exist when not set to 6
  Uint8 unkn7[3];                 // ???
  // Half of structure - 21 bytes
  Uint8 unkn8[3];                 // usually zeroes
  Uint8 status;                   // Car status;2-can enter into car
  Uint8 type;                             // Car type;13-standard
  Uint8 direction[4];             // direction the car is turned; setting all to 122 makes you die when exiting car (?)
  Uint8 unkn11[2];                // ??
  Uint8 unkn12[2];                // if both set to 122, there is no car
  Uint8 unkn16[6];                // usually zeroes
  Uint8 speed;                    // changing this to 0 make car unable to move. The bigger, the faster car is.
  Uint8 unkn20;                   // ??
} __attribute__((packed));

struct unk05 {
  Uint8 unkn10[30];
} __attribute__((packed));

struct unk06 {
  Uint8 unkn10[36];
} __attribute__((packed));

struct unk07 {
  Uint8 unkn10[12];
} __attribute__((packed));

struct unk08 {
  Uint8 unkn10[8];
} __attribute__((packed));

struct unk09 {
  Uint8 unkn10[437];
} __attribute__((packed));

struct unk10 {
  Uint8 unkn10[11];
  Uint16 map;
  Uint16 minx;
  Uint16 miny;
  Uint16 maxx;
  Uint16 maxy;
  Uint8 zero[4];
  Uint16 objective;
  Uint16 objective_data;
  Uint8 unkn11[14*9 - 11 - 14];
} __attribute__((packed));

// Agents table?
struct unk11 {
  Uint8 unkn10[15];
} __attribute__((packed));

struct lvl {
  struct unk01 u01;
  struct unk02 u02[128];
  struct ped   ped[256];
  struct car   car[64];
  struct unk05 u05[400];
  struct unk06 u06[725];
  struct unk07 u07;
  struct unk08 u08[2048];
  struct unk09 u09;
  struct unk10 u10;
  struct unk11 u11[129];
} __attribute__((packed));

char *obj[] = {
  "0",
  "persuade",
  "assassinate",
  "protect",
  "4",
  "grab",
  "6","7","8","9",
  "melee (cops)",
  "melee",
  "12","13",
  "raid/rescue",
  "use vehicle",
};

void report(struct lvl *l)
{
  int i;

  if(sizeof(struct ped) != 92)     printf("Wrong size peds...\n");
  if(sizeof(struct car) != 42)     printf("Wrong size cars...\n");
  if(sizeof(struct lvl) != 116014) printf("Wrong size lvls...\n");


  printf("map: %d ", l->u10.map);
  printf("X: %d .. %d\n", l->u10.minx, l->u10.maxx);
  printf("Y: %d .. %d\n", l->u10.miny, l->u10.maxy);
  printf("objective %2d objective data %6d: %12s\n",
	 l->u10.objective, l->u10.objective_data,
	 obj[l->u10.objective & 0x0F ]);

  for(i=0; i < 256; i++) {
    if(l->ped[i].pz)
      printf("ped[%3d]: %3d.%02x,  %3d.%02x,  %2d.%02x  vis:%x "
	     "A:%02x I:%02x P:%02x\n", i,
	     l->ped[i].px / 256, l->ped[i].px & 0xFF,
	     l->ped[i].py / 256, l->ped[i].py & 0xFF,
	     l->ped[i].pz / 256, l->ped[i].pz & 0xFF,
	     l->ped[i].vistype,
	     l->ped[i].adrenalevels[3],
	     l->ped[i].intelilevels[3],
	     l->ped[i].perceplevels[3]  );
  }
}

int main(int argc, char **argv)
{
  struct lvl l;
  int i;

  printf("sizeof(ped): %d\n", sizeof(struct ped)); /* want: 92*/
  printf("sizeof(car): %d\n", sizeof(struct car)); /* want: 42 */
  printf("sizeof(lvl): %d\n", sizeof(struct lvl)); /* want: 116014 */
  printf("----------------------------------------"
	 "---------------------------------------\n");

  while(--argc) {
    if((i = open(argv[argc], O_RDONLY)) < 0) {
      printf("file to eat?\n");
      exit(5);
    }
    read(i, &l, sizeof(l));
    printf("==== %s ====\n", argv[argc]);
    report(&l);
    close(i);
  }

  return 0;
}
