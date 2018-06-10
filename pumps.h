#ifndef PUMPS_H
#define PUMPS_H

#include "defines.h"

enum class DeviceRevision { Unset, SteveV1, SteveV2 };

class Pumps
{
public:
  Pumps();
  void SetRevision(DeviceRevision revision);
  DeviceRevision GetRevision() const;

  int GetPumpCount() const;

  void StartPumps(const uint32_t* pump_times_ms, int count, uint32_t time_now_ms);

  void StopAllPumps();

  void update(uint32_t time_ms);

  int GetTimeRemaining() const;

  bool isReady() const;

protected:
  void SetPump(int pump, bool on);
  void UpdatePumpsFromState();

  DeviceRevision mRevision;
  uint32_t mPumpTimesMs[MAX_PUMP_COUNT];
  uint32_t mLastUpateTimeMs;
};

#endif //PUMPS_H
