#ifndef __PHYS_PVT_H__
#define __PHYS_PVT_H__

typedef struct phys_xyz_point{
    double x;
    double y;
    double z;
}xyz_point;

typedef struct phys_enu_point{
    double east;
    double north;
    double up;
}enu_point;

typedef struct phys_rpy_point{
    double roll;
    double pitch;
    double yaw;
}rpy_point;

typedef struct phys_be_point{
    double bearing;
    double elevation;
}be_point;

/**
 * @brief channel characteristic structure
 * propagation_time
 * RSSI
 * Integrity
 */
typedef struct ch_characteristics{
    double propagation_time;
    double RSSI;
    double integrity;
    double accuracy;
}ch_params;

typedef struct time_characteristics{
    double ts_current;
    double ts_measurement;
}t_params;

#endif // __PHYS_PVT_H__