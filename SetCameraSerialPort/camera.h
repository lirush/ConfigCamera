#ifndef CAMERA_H
#define CAMERA_H
#include <QMainWindow>
#include <string.h>

enum ConfigBytes{
    BCMODE=8,
    CONTRAST=9,
    BRIGHTNESS=10,
    CONTRASTBIASL=11,
    CONTRASTBIASH=12,
    TEMPORALFILTER=13,
    VIDEOOUTPUT=14,
    GAMMA=15,
    MAXGAIN=16,
    PALETTE=17,
    ZOOM=18,
    HISTOGRAMMGROPL,
    HISTOGRAMMGROPH,
    AGCBLOCKS,
    EXTRACONTRAST,
    SHARPERING,
    DESTRIPING,
    IMAGEFLIP
};

struct ControlAndSettings{
    unsigned char zoom;
    unsigned char palette;
    unsigned char bright;
    unsigned char contrast;
    unsigned char contrastbias;
    unsigned char extracontrast;
    unsigned char gamma;
    unsigned char sharpen;
    unsigned char destripe;
};

struct Config{
    uint8_t alwayszero;
    uint32_t always1to8;
    uint8_t bcmode;
    uint8_t contrast;
    uint8_t brightness;
    uint16_t contrastbias;
    uint8_t temporalfilter;
    uint8_t videooutput;
    uint8_t gamma;
    uint8_t maximumgain;
    uint8_t palatte;
    uint8_t zoomfactor;
    uint16_t histogramcropping;
    uint8_t AGCblocks;
    uint16_t extracontrast;
    uint8_t sharpening;
    uint8_t destriping;
    uint8_t imageflip;
    uint32_t reserve0;
    uint8_t externalsync;
    uint8_t reserve1;
    uint16_t reserve2;
};

class Camera : public QObject
{
    Q_OBJECT

public:
    ControlAndSettings SettingsCam;
    Config ReadConfigCam;
    Camera()
    {
        memset(&SettingsCam,0x00,sizeof(struct ControlAndSettings));
    }
};

#endif // CAMERA_H
