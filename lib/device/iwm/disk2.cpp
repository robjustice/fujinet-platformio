#ifdef BUILD_APPLE
#include "disk2.h"

#include "fnSystem.h"
#include "led.h"
#include "fuji.h"
#include "fnHardwareTimer.h"

const int8_t phases_lut [16][16] = {
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 2, 1, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0},
{ 0,-2, 0,-1, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0,-1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0},
{ 0, 0,-2, 0, 0, 0,-1, 0, 2, 0, 0, 0, 1, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0,-1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 2, 0, 0,-2, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0},
{ 0, 1, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0,-1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

iwmDisk2::~iwmDisk2()
{
}

void iwmDisk2::shutdown()
{
}

iwmDisk2::iwmDisk2()
{
  track_pos = 2;
  old_pos = 0;
  oldphases = 0;
  Debug_printf("\nNew Disk ][ object");
}

mediatype_t iwmDisk2::mount(FILE *f)//, const char *filename), uint32_t disksize, mediatype_t disk_type)
{

  mediatype_t mt = MEDIATYPE_UNKNOWN;
  mediatype_t disk_type = MEDIATYPE_WOZ;

  // Debug_printf("disk MOUNT %s\n", filename);

  // Destroy any existing MediaType
  if (_disk != nullptr)
  {
    delete _disk;
    _disk = nullptr;
  }

    // Determine MediaType based on filename extension
    // if (disk_type == MEDIATYPE_UNKNOWN && filename != nullptr)
    //     disk_type = MediaType::discover_mediatype(filename);

    switch (disk_type)
    {
    case MEDIATYPE_WOZ:
        Debug_printf("\nMedia Type WOZ");
        device_active = true;
        _disk = new MediaTypeWOZ();
        mt = ((MediaTypeWOZ *)_disk)->mount(f);
        break;
    default:
        Debug_printf("\nMedia Type UNKNOWN - no mount");
        device_active = false;
        break;
    }

    return mt;

}

void iwmDisk2::unmount()
{

}

bool iwmDisk2::write_blank(FILE *f, uint16_t sectorSize, uint16_t numSectors)
{
  return false;
}

bool iwmDisk2::phases_valid(uint8_t phases)
{
  switch (phases) // lazy brute force way
  {
  case 1:
  case 2:
  case 3:
  case 4:
  case 6:
  case 8:
  case 9:
  case 12:
    return true;
  default:
    break;
  }
  return false;
}

bool iwmDisk2::move_head()
{
  int delta;
  uint8_t newphases = smartport.iwm_phase_vector(); // could access through IWM instead
  if (phases_valid(newphases))
  {
    delta = phases_lut[oldphases][newphases];
    old_pos = track_pos;
    track_pos += delta;
    if (track_pos < 0)
    {
      track_pos = 0;
    }
    else if (track_pos > MAX_TRACKS-1)
    {
      track_pos = MAX_TRACKS-1;
    }
    oldphases = newphases;
  }
  return (delta != 0);
}

void iwmDisk2::change_track()
{
  if (old_pos == track_pos)
    return;
  
  // need to tell diskii_xface the number of bits in the track
  // and where the track data is located so it can convert it
  diskii_xface.encode_spi_packet(
    ((MediaTypeWOZ*)_disk)->get_track(track_pos),
    ((MediaTypeWOZ*)_disk)->track_len(track_pos),
    ((MediaTypeWOZ*)_disk)->num_bits(track_pos)
    );


}

void iwmDisk2::update_spi_queue()
{
  
}


void iwmDisk2::process()
{

}

#endif /* BUILD_APPLE */