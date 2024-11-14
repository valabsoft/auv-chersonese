#ifndef ENUMCLASSES_H
#define ENUMCLASSES_H

enum CameraView : int { MONO, STEREO };
enum CameraConnection : int { OFF, ON };
enum CameraType : int { IP = 0, WEB = 1 };

enum class DISPARITY_TYPE
{
    ALL,
    BASIC_DISPARITY,
    BASIC_HEATMAP,
    FILTERED_DISPARITY,
    FILTERED_HEATMAP,
};

#endif // ENUMCLASSES_H
