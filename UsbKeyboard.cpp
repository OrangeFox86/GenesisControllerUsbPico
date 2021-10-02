#include "UsbKeyboard.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

UsbKeyboard::UsbKeyboard(uint8_t interfaceId, uint8_t reportId) :
  interfaceId(interfaceId),
  reportId(reportId),
  currentKeycodes(),
  keycodesUpdated(false),
  keyPressed(false)
{}

bool UsbKeyboard::isButtonPressed()
{
  return keyPressed;
}

//--------------------------------------------------------------------+
// EXTERNAL API
//--------------------------------------------------------------------+
bool UsbKeyboard::updatePressed(uint8_t keycode)
{
  bool foundDuplicate = false;
  int32_t slot = -1;
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  for (uint32_t i = 0; i < sizeof(currentKeycodes) / sizeof(currentKeycodes[0]); ++i)
  {
    if (slot == -1 && currentKeycodes[i] == 0)
    {
      slot = i;
    }
    else if (currentKeycodes[i] == keycode)
    {
      foundDuplicate = true;
    }
  }
  if (slot != -1 && !foundDuplicate)
  {
    keyPressed = true;
    currentKeycodes[slot] = keycode;
    keycodesUpdated = true;
  }
  return (slot != -1 || foundDuplicate);
}

bool UsbKeyboard::updateReleased(uint8_t keycode)
{
  bool found = false;
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  for (uint32_t i = 0; i < sizeof(currentKeycodes) / sizeof(currentKeycodes[0]); ++i)
  {
    if (found)
    {
      // Shift everything 1 to the left once key is found
      currentKeycodes[i - 1] = currentKeycodes[i];
      currentKeycodes[i] = 0;
    }
    else if (currentKeycodes[i] == keycode)
    {
      // Remove this key
      found = true;
      currentKeycodes[i] = 0;
      keycodesUpdated = true;
      // If this is the first position and the next position is empty, assume no more keys pressed
      if (i == 0 && currentKeycodes[1] == 0)
      {
        keyPressed = false;
      }
    }
  }
  return found;
}

void UsbKeyboard::updateAllReleased()
{
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  memset(currentKeycodes, 0, sizeof(currentKeycodes));
  keyPressed = false;
  keycodesUpdated = true;
}

bool UsbKeyboard::send(bool force)
{
  if (keycodesUpdated || force)
  {
    bool sent = sendReport(interfaceId, reportId);
    if (sent)
    {
      keycodesUpdated = false;
    }
    return sent;
  }
  else
  {
    return true;
  }
}

uint8_t UsbKeyboard::getReportSize()
{
  return sizeof(hid_keyboard_report_t);
}

void UsbKeyboard::getReport(uint8_t *buffer, uint16_t reqlen)
{
  // Build the report
  hid_keyboard_report_t report;
  report.modifier = 0; // This class doesn't allow modifier keys
  report.reserved = 0;
  memcpy(report.keycode, currentKeycodes, sizeof(report.keycode));
  // Copy report into buffer
  uint16_t setLen = (sizeof(report) <= reqlen) ? sizeof(report) : reqlen;
  memcpy(buffer, &report, setLen);
}
