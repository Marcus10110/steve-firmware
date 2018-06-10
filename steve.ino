#include "defines.h"
#include "pumps.h"

//cloud variables.
String deviceName = "";
String revisionName = "";
String lastError = "";
bool isReadyForNewDrink = false;
int timeRemainingMs = 0;


Pumps gPumps;


void setup()
{
  //device name stuff.
  Particle.variable("device_name", deviceName);
  Particle.subscribe("particle/device/name", deviceNameHandler);
  Particle.publish("particle/device/name");
  //device revision stuff
  revisionName = GetRevisionString();
  Particle.variable("revision", revisionName);
  Particle.function("setRevision", setRevision);

  Particle.function("StartDrink", StartDrink);
  Particle.variable("isReady", isReadyForNewDrink);
  Particle.variable("timeLeft", timeRemainingMs);
  Particle.variable("lastError", lastError);

  //attempt to load the DeviceRevision from EEPROM.
  DeviceRevision loaded_revision;
  if( ParseEeprom(loaded_revision))
  {
    gPumps.SetRevision(loaded_revision);
    revisionName = GetRevisionString();
  }
}

void loop()
{
  gPumps.update(millis());
  isReadyForNewDrink = gPumps.isReady();
  timeRemainingMs = gPumps.GetTimeRemaining();
  delay(100);
}

int StartDrink( String command )
{
  uint32_t new_pump_times_s[MAX_PUMP_COUNT];

  const int pump_count = gPumps.GetPumpCount();
  if( !ParseCommand(command, pump_count, new_pump_times_s) )
    return -1;

  for( auto& pump_time : new_pump_times_s)
    pump_time *= 1000; //convert from seconds to ms.

  gPumps.StartPumps(new_pump_times_s, pump_count, millis());
  return 0;
}

bool ParseCommand( const String& command, int buffer_count, uint32_t* buffer )
{
    //expected format: "0,0,30,0", meaning a comma seperated list of times remaining. needs to send the correct number of pumps.

    //prase the command.
    int string_index = 0;
    int pump_index = 0;
    while(string_index < command.length())
    {
        if( pump_index >= buffer_count )
        {
            //error
            lastError = "pump_index >= buffer_count";
            return false;
        }

        int comma_index = command.indexOf(",", string_index);
        if( comma_index == -1)
        {
          comma_index = command.length();
        }
        int pump_value_s = command.substring(string_index, comma_index).toInt();
        string_index = comma_index + 1;
        buffer[pump_index] = (uint32_t)pump_value_s;
        pump_index++;
    }

    if(pump_index != buffer_count)
    {
        //error
        lastError = "pump_index != buffer_count";
        return false;
    }
    return true;
}

void deviceNameHandler(const char *topic, const char *data) {
    deviceName = data;
}

String GetRevisionString()
{
  switch(gPumps.GetRevision())
  {
    case DeviceRevision::Unset:
      return "Unset";
    case DeviceRevision::SteveV1:
      return "SteveV1";
    case DeviceRevision::SteveV2:
      return "SteveV2";
  }
}

int setRevision(String rev)
{
  DeviceRevision revision;
  if( rev == "Unset")
    revision = DeviceRevision::Unset;
  else if( rev == "SteveV1")
    revision = DeviceRevision::SteveV1;
  else if( rev == "SteveV2")
    revision = DeviceRevision::SteveV2;
  else
  {
    lastError = "invalid revision";
    return -1;
  }

  gPumps.SetRevision(revision);
  revisionName = GetRevisionString();
  WriteEepromRevision();
  return 0;
}

//EEPROM operations;
void WriteEepromRevision()
{
  uint8_t contents[10];
  const DeviceRevision revision = gPumps.GetRevision();
  GetRevisionString().getBytes(contents, 10);

  EEPROM.put(0, contents);
}

bool ParseEeprom(DeviceRevision& loaded_revision)
{
  char buffer[10];
  EEPROM.get(0, buffer);
  if( strcmp(buffer,"Unset") == 0 )
  {
    loaded_revision = DeviceRevision::Unset;
    return true;
  }
  else if( strcmp(buffer,"SteveV1") == 0 )
  {
    loaded_revision = DeviceRevision::SteveV1;
    return true;
  }
  else if( strcmp(buffer,"SteveV2") == 0 )
  {
    loaded_revision = DeviceRevision::SteveV2;
    return true;
  }

  lastError = String("parse error: ") + buffer;
  return false;
}
