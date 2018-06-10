#include "pumps.h"
#include "defines.h"

const int v1_pump_pins[] = {V1_PUMP0, V1_PUMP1, V1_PUMP2, V1_PUMP3, V1_PUMP4, V1_PUMP5, V1_PUMP6, V1_PUMP7, V1_PUMP8, V1_PUMP9, V1_PUMP10, V1_PUMP11};
const int v2_pump_pins[] = {V2_PUMP0, V2_PUMP1, V2_PUMP2, V2_PUMP3};

Pumps::Pumps()
: mRevision( DeviceRevision::Unset),
mLastUpateTimeMs(0)
{
  for( int i = 0; i < MAX_PUMP_COUNT; i++)
    mPumpTimesMs[i] = 0;
}

void Pumps::SetRevision(DeviceRevision revision)
{
  if( revision == mRevision)
    return;

  StopAllPumps();

  //turn old pins into inputs.
  if( mRevision == DeviceRevision::SteveV1)
  {
    for( int pin : v1_pump_pins )
      pinMode(pin, INPUT);
  }
  else if( mRevision == DeviceRevision::SteveV2)
  {
    for( int pin : v2_pump_pins )
      pinMode(pin, INPUT);
  }


  mRevision = revision;

  //setup new pins as outputs.
  if( mRevision == DeviceRevision::SteveV1)
  {
    for( int pin : v1_pump_pins )
      pinMode(pin, OUTPUT);
  }
  else if( mRevision == DeviceRevision::SteveV2)
  {
    for( int pin : v2_pump_pins )
      pinMode(pin, OUTPUT);
  }

  StopAllPumps();
}

DeviceRevision Pumps::GetRevision() const
{
  return mRevision;
}

int Pumps::GetPumpCount() const
{
  if( mRevision == DeviceRevision::SteveV1)
    return 12;
  else if( mRevision == DeviceRevision::SteveV2)
    return 4;
  else
    return 0;
}

void Pumps::StartPumps(const uint32_t* pump_times_ms, int count, uint32_t time_now_ms)
{
  if( count > GetPumpCount())
    return;
  for(int i = 0; i < count; ++i)
  {
    mPumpTimesMs[i] = pump_times_ms[i];
  }
  UpdatePumpsFromState();

  mLastUpateTimeMs = time_now_ms;
}

void Pumps::StopAllPumps()
{
  const int pump_count = GetPumpCount();
  for( int i = 0; i < pump_count; ++i )
  {
    SetPump(i, false);
    mPumpTimesMs[i] = 0;
  }
}

void Pumps::update(uint32_t time_ms)
{
  if( time_ms <= mLastUpateTimeMs)
  {
    mLastUpateTimeMs = time_ms;
    return;
  }
  const uint32_t elapsed_time = time_ms - mLastUpateTimeMs;
  mLastUpateTimeMs = time_ms;

  //decrement all pumps by elapsed_time.
  const int pump_count = GetPumpCount();
  for(int i = 0; i < pump_count; i++)
  {
    mPumpTimesMs[i] -= min(mPumpTimesMs[i], elapsed_time);
  }
  UpdatePumpsFromState();
}

int Pumps::GetTimeRemaining() const
{
  uint32_t time_remaining_ms = 0;

  for(int i = 0; i < GetPumpCount(); ++i)
    time_remaining_ms = max(time_remaining_ms, mPumpTimesMs[i]);

  return time_remaining_ms;
}

bool Pumps::isReady() const
{
  if( mRevision == DeviceRevision::Unset)
    return false;

  if(GetTimeRemaining() > 0)
    return false;

  return true;
}

void Pumps::SetPump(int pump, bool on)
{
  if( pump >= GetPumpCount())
    return;

  int pin;
  if(GetRevision() == DeviceRevision::SteveV1)
    pin = v1_pump_pins[pump];
  else if( GetRevision() == DeviceRevision::SteveV2)
    pin = v2_pump_pins[pump];
  else
    return;

  if( on )
    digitalWrite(pin, PUMP_ON);
  else
    digitalWrite(pin, PUMP_OFF);
}

void Pumps::UpdatePumpsFromState()
{
  const int count = GetPumpCount();
  for(int i = 0; i < count; ++i)
  {
    SetPump(i, (mPumpTimesMs[i] > 0));
  }

}
