#ifndef STATUSENUMS
#define STATUSENUMS
namespace routines {

    // enums for possible status values of stacking or other procedures
    enum Photo {
        inactive,
        start,
        paused,
        newShutter,
        waitShutter,
        newMovement,
        delayMovement,
        executedMovement,
        debugFlash,
        completedStack
    };

    enum Video {
        stopped,
        stopping,
        running
    };

    // enums for the possible status values of the flash procedure
    enum Flash {
        flashAvailable,
        shutterLow,
        shutterHigh,
        flashRecycling,
        shutterCompleted,
        flashUnavailable,
        flashUnresponsive
    };

    // enums for the screens of the UI
    enum Screens {
        ui_Home,
        ui_Config,
        ui_Flash,
        ui_Global,
        ui_Main,
        ui_Orbis,
        ui_Photo360,
        ui_Photo360Config,
        ui_Video360,
        ui_Stack,
        ui_Target,
        ui_Manual,
        ui_Auto,
        ui_AutoConfig
    };

    // enums for various buttons where we want to check their state
    enum Buttons {
        enum_first,
        btn_flashOn,
        btn_flashOff,
        btn_distance,
        btn_startPosition,
        btn_endPosition,
        btn_shutterDelay,
        btn_arrows,
        btn_photo360Nr,
        btn_photo360Delay,
        btn_targetPosition,
        enum_last
    };
}
#endif