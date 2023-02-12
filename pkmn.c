#include <math.h>
#include <stdint.h>
#include <stdio.h>

/* placeholder MON data structure */
struct mon
{
    unsigned char rnd0;
    unsigned char rnd1;
};

/* TODO: check PokeGSC source */
enum mon_stat
{
    HP  = 1,
    POW = 2,
    DEF = 3,
    AGI = 4,
    SPW = 5,
    SDF = 6 
};

/* MON types as of Gen2 (G/S/C) */
enum mon_type
{
    NORMAL = 0,
    FIGHTING,
    FLYING,
    POISON,
    GROUND,
    ROCK,
    BIRD,
    BUG,
    GHOST,
    STEEL,

    CURSE = 0x13,
    FIRE,
    WATER,
    GRASS,
    ELECTRIC,
    PSYCHIC,
    ICE,
    DRAGON,
    DARK
};

void mon_print_type(enum mon_type t)
{
    switch(t)
    {
        case NORMAL:
        printf("NORMAL");
        break;

        case FIGHTING:
        printf("FIGHTING");
        break;

        case FLYING:
        printf("FLYING");
        break;

        case POISON:
        printf("POISON");
        break;

        case GROUND:
        printf("GROUND");
        break;

        case ROCK:
        printf("ROCK");
        break;

        case BIRD:
        printf("BIRD");
        break;

        case BUG:
        printf("BUG");
        break;

        case GHOST:
        printf("GHOST");
        break;

        case STEEL:
        printf("STEEL");
        break;

        case CURSE:
        printf("???");
        break;

        case FIRE:
        printf("FIRE");
        break;

        case WATER:
        printf("WATER");
        break;

        case GRASS:
        printf("GRASS");
        break;

        case ELECTRIC:
        printf("ELECTRIC");
        break;

        case PSYCHIC:
        printf("PSYCHIC");
        break;

        case ICE:
        printf("ICE");
        break;

        case DRAGON:
        printf("DRAGON");
        break;

        case DARK:
        printf("DARK");
        break;

        default:
        printf("INVALID");
        break;
    }
}

/* Table for possible Hidden Power (Mezameru Power) types
 * used by mon_get_mpt()
 */
enum mon_type mon_type_meza[] =
{
    FIGHTING,
    FLYING,
    POISON,
    GROUND,
    ROCK,
    BUG,
    GHOST,
    STEEL,
    FIRE,
    WATER,
    GRASS,
    ELECTRIC,
    PSYCHIC,
    ICE,
    DRAGON,
    DARK
};

/* Hidden Power type */
enum mon_type mon_get_mpt(struct mon *m) 
{ 
    unsigned char tmp; 
    tmp = ((m->rnd0 >> 4) & 0x03) << 2;
    tmp |= (m->rnd0 & 0x03);

    return mon_type_meza[tmp];
}

/* Hidden Power power */
unsigned char mon_get_mpp(struct mon *m)
{
    unsigned char tmp;

    tmp = (m->rnd0 & 0x80) >> 4;
    tmp |= (m->rnd0 & 0x08) >> 1;

    tmp |= (m->rnd1 & 0x80) >> 6;
    tmp |= (m->rnd1 & 0x08) >> 3;

    tmp *= 5;
    tmp += m->rnd1 & 0x03;
    tmp >>= 1;
    tmp += 31;
    return tmp;
}

uint16_t mon_get_stat(void)
{

}

/* rare (shiny) MON if its POWrnd has b1 set, others at 10 (1010b)
 * rare MON if RND0: --1-1010b and RND1: 10101010b
 * possible POW values:
 * 0010 = 2
 * 0011 = 3
 * 0110 = 6
 * 0111 = 7
 * 1010 = 10
 * 1011 = 11
 * 1110 = 14
 * 1111 = 15
 * RND0 b7:b4 = POWrnd (Atk)
 * RND0 b3:b0 = DEFrnd (Def)
 * RND1 b7:b4 = AGIrnd (Speed)
 * RND1 b3:b0 = SPCrnd (Special)
 *
 * HPrnd is not stored, but derived from POW/DEF/AGI/SPCrnd:
 * b3: POWrnd & 1, b2: DEFrnd & 1
 * b1: AGIrnd & 1, b0: SPCrnd & 1
 */
unsigned char mon_is_rare(struct mon *m)
{
    /* check POW/DEF
     * 0x2f is 00101111, 0x2a is 00101010 */
    if( (m->rnd0 & 0x2f) != 0x2a ) { return 1; }

    /* check AGI/SPC
     * 0xaa is 10101010 */
    if( m->rnd1 != 0xaa ) { return 1; }
   
    /* checks passed, the MON is rare */
    return 0;
}

/* make a MON rare by setting b1 of its POWrnd
 * and DEF/AGI/SPCrnd to 0xa (refer to mon_is_rare)
 * note that there isn't a mon_clear_rare function */
void mon_set_rare(struct mon *m)
{
    /* amend POWrnd/DEFrnd
     * clear DEFrnd to prepare for bit-setting */
    m->rnd0 &= 0xf0; 
    /* set POWrnd/DEFrnd */ 
    m->rnd0 |= 0x2a; 

    /* set AGIrnd/SPCrnd */ 
    m->rnd1 = 0xaa;
} 

/* returns ceil(sqrt(stat_exp)), capped at 255 */ 
/* unsigned char mon_get_stat_exp_bonus(uint16_t stat_exp)
 * { 
 *   unsigned char i;
 *   for(i=0; i<255; i++)
 *   {
 *       if( stat_exp == pow(i,2) ) { break; }
 *       else if( stat_exp < pow(i,2) ) { break; } 
 *   }
 *   return i;
 * } 
 */

unsigned char mon_get_stat_exp_bonus(uint16_t stat_exp)
{
    double d;
    d = ceil(sqrt(stat_exp));
    return( d > 255 ? 255 : (unsigned char) d );
}


void mon_print_rare(struct mon *m)
{
    if( !mon_is_rare(m) ) { puts("rare MON"); }
    else { puts("sucky MON"); } 
}

void mon_print_meza(struct mon *m)
{
    mon_print_type(mon_get_mpt(m));
    printf(" %u\n", mon_get_mpp(m));
}
 
int main(int argc, char **argv)
{
    struct mon m;
    /* Pikachu from Bulbagarden Hidden Power calculation example
     * HPrnd: 14, POWrnd: 15, DEFrnd: 15, AGIrnd: 15, SPCrnd: 14
     */
    m.rnd0 = 0xff;
    m.rnd1 = 0xfe;

    mon_print_rare(&m); 
    mon_print_meza(&m);

    puts("now set rare");
    mon_set_rare(&m);

    mon_print_rare(&m);
    mon_print_meza(&m); 
    
    return 0;
}


