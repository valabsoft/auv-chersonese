#pragma once
#ifndef __PHYS_H
#define __PHYS_H

#include "phys_pvt.h"
#include <stdint.h>
#include <time.h>

typedef struct phys_usbl_long{
    xyz_point xyz;
    enu_point enu;
    rpy_point rpy;
    ch_params ch;
    t_params t;
    uint8_t MAC;
}usbl_long_t;

typedef struct phys_usbl_angles{
    be_point l_target;
    be_point target;
    rpy_point rpy;
    ch_params ch;
    t_params t;
    uint8_t MAC;
}usbl_angles_t;

typedef struct phys_usbl_recvim{
    uint8_t src;
    uint8_t dest;
    uint8_t fm;
    uint8_t brdcst;
    uint32_t len;
    char *phy_ack_flag;
    char *payload;
    char *command;
    double tr_duration;
    double rel_velocity;
    ch_params ch;
}recvim_t;

typedef struct phys_usbl_phyp{
    t_params t;
    uint8_t MAC;
    uint8_t fix_type;
    xyz_point t_123;
    xyz_point t_432;
    xyz_point t_341;
    xyz_point t_412;
    xyz_point t_153;
    xyz_point t_254;
}usbl_phyp_t;

typedef struct phys_usbl_phyd{
    t_params t;
    uint8_t MAC;
    uint8_t fix_type;
    double d15;
    double d25;
    double d35;
    double d45;
    double d12;
    double d14;
    double d23;
    double d34;
}usbl_phyd_t;

typedef struct at_buffer{
    usbl_phyp_t phyp;
    usbl_phyd_t phyd;
    usbl_angles_t last_usbla;
    usbl_long_t last_usbll;
    recvim_t recv_params;
    uint8_t last_com;
    time_t timestamp;
}at_buff_t;

#endif // __PHYS_H