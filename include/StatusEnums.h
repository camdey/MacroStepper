#ifndef STATUSENUMS
#define STATUSENUMS
namespace routines {
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

    enum Flash {
        flashAvailable,
        shutterLow,
        shutterHigh,
        flashRecycling,
        shutterCompleted,
        flashUnavailable,
        flashUnresponsive
    };
}
#endif