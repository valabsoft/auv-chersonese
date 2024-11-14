/**
 * @file codes.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-05
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#ifndef _CODES_H
#define _CODES_H

#define OK_CODE                 0
#define WRONG_FORMAT_CODE       0
#define BUSY_CODE               0
#define FAILED_CODE             0
#define USBLANGLES_CODE         105
#define USBLLONG_CODE           104
#define USBLPHYP_CODE           111
#define USBLPHYD_CODE           112
#define DELIVER_CODE            7
#define RECIEVE_CODE            2
#define SEND_CODE               6

#ifdef TLOHI
    #define DELIVER_CODE            117
    #define RECIEVE_CODE            12
    #define SEND_CODE               116
#endif
#define PARSED_CODE             0
#define FOR_ME_CODE             113  //1
#define UNKNOWN_CODE            19
#define NOTHING_HAPPENED_CODE   19
#define BROADCAST               255

#endif