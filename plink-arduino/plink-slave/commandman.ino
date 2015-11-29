//0 = NO COMMAND
//1 = TURN ON ALERT
//2 = DISMISS ALL ALERTS

uint8_t generateEnableAlertCmd()
{
  return 1;
}

uint8_t generateDismissAlertCmd()
{
  return 2;
}

uint8_t generateConfirmEnableAlertCmd()
{
  return 3;
}

uint8_t generateConfirmDismissAlertCmd()
{
  return 4;
}
