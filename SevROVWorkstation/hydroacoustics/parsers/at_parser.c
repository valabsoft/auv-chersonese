#include "at_parser.h"

#include "common_parser.h"
#include "../codes/codes.h"
#include "../codes/phys.h"

static at_struct str_list[] = {
    {(char*)"RECV",                 RECV},
    {(char*)"RECVIM",               RECVIM},
    {(char*)"DELIVERED",            DELIVERED},
    {(char*)"DELIVEREDIM",          DELIVEREDIM},
    {(char*)"USBLLONG",             USBLLONG},
    {(char*)"USBLANGLES",           USBLANGLES},
    {(char*)"OK\r",                 OK},
    {(char*)"BUSY",                 BUSY},
    {(char*)"FAILED",               FAILED},
    {(char*)"FAILEDIM",             FAILEDIM},
    {(char*)"ERROR WRONG FORMAT\r", EWRONGFORMAT},
    {(char*)"USBLPHYP",             USBLPHYP},
    {(char*)"USBLPHYD",             USBLPHYD},
};

at_enum at_chooser(char chunks[MAX_COMMANDS][MAX_CHARACTERS]){
    char *command = chunks[0];
    at_enum comm_enum = UNKNOWN;
    at_struct *cs = NULL;
    for(int i = 0; i<NUM_COMMANDS; i++)
    {
        if(strcmp(command, str_list[i].str)==0)
        {
            cs=str_list+i;
            comm_enum = cs->en;
            break;
        }
    }
    return comm_enum;
}

int at_handler(char input_line[MAX_CHARACTERS], int id, void *out){
    char chunks[MAX_COMMANDS][MAX_CHARACTERS];
    comma_parser(input_line, (char *)chunks);
    at_enum comm_enum = at_chooser(chunks);
    recvim_t recv;
    usbl_long_t point_long;
    usbl_angles_t point_angles;
    usbl_phyp_t phyp;
    usbl_phyd_t phyd;

    if(comm_enum == RECVIM){
        recv.len            = ichunk(AT_PAYLOAD_LEN);
        recv.src            = ichunk(AT_SOURCE);
        recv.brdcst         = (recv.src == BROADCAST);
        recv.dest           = ichunk(AT_DESTINATION);
        recv.fm             = (recv.dest == id);
        recv.phy_ack_flag   = chunks[AT_ACK_FLAG];
        recv.tr_duration    = fchunk(AT_DURATION)/1000000.0;
        recv.ch             = (ch_params){0.0, fchunk(6), fchunk(7), 0.0};
        recv.rel_velocity   = fchunk(AT_VELOCITY);
        recv.payload        = strdup(chunks[AT_PAYLOAD]);
        char *duplicate     = strdup(recv.payload);
        //recv.command        = strsep(&duplicate, ";"); // on Unix
        recv.command        = strtok(duplicate, ";"); // on Windows
        memcpy(out,&recv,sizeof(recvim_t));
        return RECIEVE_CODE;
    }

    if(comm_enum == RECV)
        return RECIEVE_CODE;

    if((comm_enum == DELIVERED) || (comm_enum == DELIVEREDIM)){
        return DELIVER_CODE;
    }

    if(comm_enum == USBLLONG){
        point_long.xyz = (xyz_point){fchunk(4), fchunk(5), fchunk(6)};
        point_long.enu = (enu_point){fchunk(7), fchunk(8), fchunk(9)};
        point_long.rpy = (rpy_point){fchunk(10),fchunk(11),fchunk(12)};
        point_long.ch  = (ch_params){fchunk(13),fchunk(14),fchunk(15),fchunk(16)};
        point_long.t   = (t_params) {fchunk(1), fchunk(2)};
        point_long.MAC = atoi(chunks[3]);
        memcpy(out,&point_long,sizeof(usbl_long_t));
        return USBLLONG_CODE;
    }

    if(comm_enum == USBLANGLES){
        point_angles.l_target   = (be_point) {fchunk(4),fchunk(5)};
        point_angles.target     = (be_point) {fchunk(6),fchunk(7)};
        point_angles.rpy        = (rpy_point){fchunk(8),fchunk(9),fchunk(10)};
        point_angles.ch         = (ch_params){0,fchunk(11),fchunk(12),fchunk(13)};
        point_angles.t          = (t_params) {fchunk(1),fchunk(2)};
        point_angles.MAC        = atoi(chunks[3]);
        memcpy(out,&point_angles,sizeof(usbl_angles_t));
        return USBLANGLES_CODE;
    }

    if(comm_enum == USBLPHYP){
        phyp.t  = (t_params) {fchunk(1),fchunk(2)};
        phyp.MAC = atoi(chunks[3]);
        phyp.fix_type = atoi(chunks[4]);
        phyp.t_123 = (xyz_point){fchunk(5), fchunk(6), fchunk(7)};
        phyp.t_432 = (xyz_point){fchunk(8), fchunk(9), fchunk(10)};
        phyp.t_341 = (xyz_point){fchunk(11), fchunk(12), fchunk(13)};
        phyp.t_412 = (xyz_point){fchunk(14), fchunk(15), fchunk(16)};
        phyp.t_153 = (xyz_point){fchunk(17), fchunk(18), fchunk(19)};
        phyp.t_254 = (xyz_point){fchunk(20), fchunk(21), fchunk(22)};
        memcpy(out,&phyp,sizeof(usbl_phyp_t));
        return USBLPHYP_CODE;
    }

    if(comm_enum == USBLPHYD){
        phyd.t  = (t_params) {fchunk(1),fchunk(2)};
        phyd.MAC = atoi(chunks[3]);
        phyd.fix_type = atoi(chunks[4]);
        phyd.d15 = fchunk(5);
        phyd.d25 = fchunk(6);
        phyd.d35 = fchunk(7);
        phyd.d45 = fchunk(8);
        phyd.d12 = fchunk(9);
        phyd.d14 = fchunk(10);
        phyd.d23 = fchunk(11);
        phyd.d34 = fchunk(12);
        memcpy(out,&phyd,sizeof(usbl_phyd_t));
        return USBLPHYD_CODE;
    }

    if(comm_enum == OK){
        //printf("OK\n");
        return OK_CODE;
    }

    if((comm_enum == BUSY) || (comm_enum == FAILED) || (comm_enum == FAILEDIM)){
        return BUSY_CODE;
    }

    if(comm_enum == EWRONGFORMAT){
        printf("НЕ ТОТ ФОРМАТ\n");
        return WRONG_FORMAT_CODE;
    }
    return NOTHING_HAPPENED_CODE;
}

size_t send_at(char *sendline, int addr, char* mes, int ack){
    sprintf(sendline,"AT*SENDIM,%li,%i,%s,%s\n",
            strlen(mes),addr,ack?"ack":"noack",mes);
    return 0;
}

//generator
int gen_message(char* generated_message, char* message, int address, int flags, int (*prot_payload)(char*, char*, int)){
    int return_code = DELIVER_CODE;
    char sendline[4096] = { 0 };
    char payload[4096-40] = {0};

    return_code = prot_payload(payload, message, flags);
    int payload_length = strlen(payload);

    sprintf(sendline,"AT*SENDIM,%i,%i,",payload_length,address);

    if(flags & ACK_MASK){
        strcat(sendline,"ack,");
    }else{
        strcat(sendline,"noack,");
    }
    strcat(sendline,payload);
    strcat(sendline,"\n");
    strcpy(generated_message,sendline);
    return return_code;
}

int polling(char* generated_message){
    int return_code = DELIVER_CODE;
    char sendline[4096] = { 0 };
    char polling_mess[4096-40] = "POLLING_DATA";
    int payload_length = strlen(polling_mess);

    sprintf(sendline,"AT*SENDIM,%i,%i,noack,",payload_length,BROADCAST); //255 is for everyone
    strcat(sendline,polling_mess);
    strcat(sendline,"\n");
    strcpy(generated_message,sendline);
    return return_code;
}

void at_extended(char* generated_message, bool flag){
    char sendline[4096] = { 0 };
    sprintf(sendline,"AT@ZX%i\n",flag);
    strcpy(generated_message,sendline);
}

void at_positioning(char* generated_message, bool flag){
    char sendline[4096] = { 0 };
    sprintf(sendline,"AT@ZU%i\n",flag);
    strcpy(generated_message,sendline);
}